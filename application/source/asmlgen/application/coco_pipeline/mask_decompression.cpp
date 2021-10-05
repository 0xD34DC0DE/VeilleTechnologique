#include "asmlgen/application/coco_pipeline/mask_decompression.h"
#include "asmlgen/application/config/assertion.h"

#include <algorithm>
#include <cassert>
#include <cmath>

namespace pipeline
{

CocoDecompressionData* MaskDecompression::Call(CocoDecompressionData* coco_decompression_data)
{
  const uint32_t image_width = coco_decompression_data->GetImageWidth();
  const uint32_t image_height = coco_decompression_data->GetImageHeight();

  const std::vector<std::string>& mask_id_values_name = *coco_decompression_data->GetMaskIdValuesName();
  ASSERT(mask_id_values_name.size() < 255, "They are more than 255 categories");

  std::vector<std::pair<std::string, std::vector<AnnotationData*>>> annotations_groups;
  auto& annotations = coco_decompression_data->GetAnnotationsData();
  for (int i = 0; i < annotations.size(); i++)
  {
    bool found = false;
    for (auto& annotation_group : annotations_groups)
    {
      if (annotation_group.first == annotations[i].first)
      {
        auto* ptr = const_cast<AnnotationData*>(&annotations[i]);
        annotation_group.second.emplace_back(ptr);
        found = true;
        break;
      }
    }
    if (!found)
    {
      annotations_groups.emplace_back(std::make_pair(
        annotations[i].first, std::vector<AnnotationData*> { const_cast<AnnotationData*>(&annotations[i]) }));
    }
  }

  std::vector<std::pair<std::string, std::vector<uint8_t>>> grouped_masks;

  for (const auto& annotation_group : annotations_groups)
  {
    std::vector<uint8_t> combined_mask(image_width * image_height * 3, 0);
    for (const auto& annotation_data : annotation_group.second)
    {
      std::vector<uint8_t> decompressed_mask =
        DecompressAnnotationToMask(image_width, image_height, annotation_data->second);

      uint32_t mask_value = 0;
      std::string output_path = coco_decompression_data->GetFilepath();

      for (int i = 0; i < mask_id_values_name.size(); ++i)
      {
        if (mask_id_values_name[i] == annotation_data->first)
        {
          mask_value = i;
          break;
        }
      }

      mask_value = ((mask_value >> 16) ^ mask_value) * 0x45d9f3b;
      mask_value = ((mask_value >> 16) ^ mask_value) * 0x45d9f3b;
      mask_value = (mask_value >> 16) ^ mask_value;

      for (size_t i = 0; i < decompressed_mask.size(); ++i)
      {
        if (decompressed_mask[i])
        {
          combined_mask[i * 3 + 0] |= mask_value & 0xFF;
          combined_mask[i * 3 + 1] |= (mask_value & 0xFF00) >> 8;
          combined_mask[i * 3 + 2] |= (mask_value & 0xFF0000) >> 16;
        }
      }
    }

    grouped_masks.emplace_back(std::make_pair(annotation_group.first, std::move(combined_mask)));
  }

  coco_decompression_data->SetMaskBytes(std::move(grouped_masks));

  coco_decompression_data->SetValid(true);

  return coco_decompression_data;
}

std::vector<uint8_t> MaskDecompression::DecompressAnnotationToMask(
  unsigned int width, unsigned int height, const SegmentationData& segmentation)
{
  // TODO decompress all annotation of the same category id together
  // TODO convert std::vector<bool> to std::vector<uint8_t>
  // TODO convert std::vector<float> of RLE to std::vector<uint32_t>
  if (segmentation.index() == 0)
  {
    std::vector<Edge> polygonsEdges = GenerateEdges(segmentation);
    return ScanlineFill(width, height, polygonsEdges);
  }
  else
  {
    return RleFill(width, height, std::get<std::vector<float>>(segmentation));
  }
}

struct MaskDecompression::Edge
{
  float x1, y1, x2, y2;
  float inverseSlope;
  float yIntercept;
  bool isVertical = false;

  Edge(float x1, float y1, float x2, float y2) : x1(x1), y1(y1), x2(x2), y2(y2)
  {
    sortByYAscending();
    // Vertical lines have an undefined slope and no y-intercept when in the form y=ax+b
    if (x2 == x1)
    {
      isVertical = true;
      return;
    }
    float slope = (y2 - y1) / (x2 - x1);
    yIntercept = y1 - slope * x1;
    inverseSlope = 1.0f / slope;
  }

  Edge& sortByYAscending()
  {
    if (y1 > y2)
    {
      std::swap(x1, x2);
      std::swap(y1, y2);
    }
    return *this;
  }

  // This method requires that the edge is vertex sorted by ascending y
  [[nodiscard]] bool isScanlineInsideYRange(unsigned int scanlineY) const
  {
    return static_cast<float>(scanlineY) >= y1 && static_cast<float>(scanlineY) <= y2;
  }

  [[nodiscard]] float getXInterceptWithScanline(unsigned int scanlineY) const
  {
    if (isVertical) { return x1; }
    return (static_cast<float>(scanlineY) - yIntercept) * inverseSlope;
  }
};

std::vector<MaskDecompression::Edge> MaskDecompression::GenerateEdges(const SegmentationData& segmentationData)
{
  std::vector<Edge> polygonEdges;

  assert(segmentationData.index() == 0);

  // For each polygon
  for (const std::vector<float>& coordinates : std::get<PolygonCoordinatesArrays>(segmentationData))
  {

    // Construct edge from 2 consecutive coordinates
    unsigned int length = coordinates.size();
    for (unsigned int i = 0; i < length - 2; i += 2)
    {
      polygonEdges.emplace_back(coordinates[i + 0], coordinates[i + 1], coordinates[i + 2], coordinates[i + 3]);
    }

    // Create last edge from last coordinate to the first
    polygonEdges.emplace_back(coordinates[length - 2], coordinates[length - 1], coordinates[0], coordinates[1]);
  }

  return polygonEdges;
}

std::vector<uint8_t> MaskDecompression::ScanlineFill(unsigned int width, unsigned int height, std::vector<Edge>& edges)
{
  std::vector<uint8_t> mask;
  mask.resize(width * height, false);

  // Sort edges by their y coordinate (y midpoint) by ascending order
  std::sort(edges.begin(),
    edges.end(),
    [](Edge edge1, Edge edge2) { return (edge1.y1 + edge1.y2) * 0.5f < (edge2.y1 + edge2.y2) * 0.5f; });

  // Sort each edge vertices to be in ascending order by their y
  std::for_each(edges.begin(), edges.end(), [](Edge& edge) { edge.sortByYAscending(); });

  // Step thought every y increment of all edges (scanline)

  // TODO make activeEdges and remainingEdges static vectors to prevent recreation of it at every polygon fill
  // TODO Use linked list instead of vector (Benchmarks ?)
  // Keeps track of active edges
  // An edge is added to the vector if the scanline is in between the start and end of the edge
  std::vector<Edge*> activeEdges;
  activeEdges.reserve(edges.size());

  // Keeps track of edges not yet reached by the scanline
  // Allows for minimal testing of which edges are now active
  std::vector<Edge*> remainingEdges;
  remainingEdges.reserve(edges.size());
  for (Edge& edge : edges)
  {
    Edge* e = &edge;
    remainingEdges.emplace_back(e);
  }

  // Min and max y coordinate from all edges
  auto start = static_cast<unsigned int>(std::floor(edges.begin()->y1));
  auto end = static_cast<unsigned int>(std::ceil(edges.back().y2));

  unsigned int y = 0;
  unsigned int* yPtr = &y;

  bool activeEdgesNeedsSorting =
    true; // Avoids sorting after each insertion inside activeEdges, just after all insertions

  // Used by std::remove_if by returning true for elements to be deleted
  // Element to be deleted are transferred to the activeEdges vector
  auto remainingToActiveTransferLambda = [&activeEdges, yPtr, &activeEdgesNeedsSorting](Edge* edge)
  {
    if (edge->isScanlineInsideYRange(*yPtr))
    {
      activeEdges.emplace_back(edge);
      activeEdgesNeedsSorting = true;
      return true;
    }
    return false;
  };

  // Used by std::remove_if by returning true for elements to be deleted
  // Element to be deleted are removed from the activeEdges vector when they are past the scanline
  auto activeRemoveLambda = [yPtr](Edge* edge)
  {
    if (!edge->isScanlineInsideYRange(*yPtr)) { return true; }
    return false;
  };

  // Buffer for all x locations of edge intercepts with the scanline
  std::vector<float> interceptsXCoords;
  interceptsXCoords.reserve(edges.size());

  for (y = start; y < end; y++)
  {
    // Transfer edges by deleting them from the remainingEdge vector and adding them to the activeEdges vector
    remainingEdges.erase(std::remove_if(remainingEdges.begin(), remainingEdges.end(), remainingToActiveTransferLambda),
      remainingEdges.end());

    // Remove edges that are past the scanline
    activeEdges.erase(std::remove_if(activeEdges.begin(), activeEdges.end(), activeRemoveLambda), activeEdges.end());

    // Sort edges from left to right if there was a change to the activeEdges vector
    if (activeEdgesNeedsSorting)
    {
      std::sort(activeEdges.begin(),
        activeEdges.end(),
        [](Edge* edge1, Edge* edge2) { return (edge1->x1 + edge1->x2) * 0.5f < (edge2->x1 + edge2->x2) * 0.5f; });
      activeEdgesNeedsSorting = false;
    }

    // Get x coordinates of all edge intercepts with the scanline
    interceptsXCoords.clear();
    for (const Edge* edge : activeEdges)
    {
      interceptsXCoords.emplace_back(edge->getXInterceptWithScanline(y));
    }

    // Fill the pixels in between each pair of x coordinates. Regions inside the polygon are defined by pairs
    // of x coordinate, such that a coordinate with an odd index will make a pair with the next coordinate with
    // an even index
    if (!interceptsXCoords.empty())
    {
      for (unsigned int i = 0; i < interceptsXCoords.size() - 1; i++)
      {
        if (i + 1 % 2 != 0)
        {
          auto xStart = static_cast<unsigned int>(std::floor(interceptsXCoords[i]));
          auto xEnd = static_cast<unsigned int>(std::ceil(interceptsXCoords[i + 1]));

          for (unsigned int x = xStart; x < xEnd; x++)
          {
            mask[x + y * width] = true;
          }
        }
      }
    }
  }

  return mask;
}

std::vector<uint8_t> MaskDecompression::RleFill(unsigned int width, unsigned int height, const std::vector<float>& rle)
{
  std::vector<uint8_t> mask;
  mask.resize(width * height, false);

  unsigned int position = 0;
  bool isDrawing = false;

  // Decode RLE by reading odd values with an odd index as jumps and even index as numbers of pixels to fill
  for (const float rleVal : rle)
  {
    if (isDrawing)
    {
      auto pixelsToDraw = static_cast<unsigned int>(rleVal);
      for (unsigned int i = 0; i < pixelsToDraw; i++)
      {
        // Instead of left to right like most RLE's, the COCO dataset's RLE is top to bottom so a
        // conversion is needed to get the right index
        unsigned int x = (position + i) / height;
        unsigned int y = (position + i) % height;
        mask[x + y * width] = true;
      }
      position += pixelsToDraw;
    }
    else
    {
      position += static_cast<unsigned int>(rleVal);
    }

    isDrawing = !isDrawing;
  }

  return mask;
}

} // namespace pipeline
