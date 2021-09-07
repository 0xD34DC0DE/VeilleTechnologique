#ifndef ASMLGEN_COCO_DATASET_TYPES_H
#define ASMLGEN_COCO_DATASET_TYPES_H

#include <string>
#include <variant>

namespace dataset
{

struct COCOImage
{
  uint64_t id;
  std::string COCOUrl;
  std::string flickrUrl;
  uint32_t width;
  uint32_t height;
  std::string filename;
};

using PolygonCoordinatesArrays = std::vector<std::vector<float>>;
using SegmentationData = std::variant<PolygonCoordinatesArrays, std::vector<float>>;

struct COCOAnnotation
{
  uint64_t id;
  uint32_t categoryId;
  bool isCrowd;
  SegmentationData segmentation;
  std::pair<unsigned int, unsigned int> size;
  uint64_t imageId;
  const COCOImage* cocoImagePtr = nullptr;
};

struct COCOCategory
{
  std::string supercategory;
  uint32_t id;
  std::string name;
};

} // namespace dataset

#endif // ASMLGEN_COCO_DATASET_TYPES_H
