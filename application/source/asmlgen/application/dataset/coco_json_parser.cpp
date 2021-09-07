#include "asmlgen/application/dataset/coco_json_parser.h"

#include "asmlgen/application/dataset/json_parsing_helpers.h"

namespace dataset
{

uint64_t CocoJsonParser::LoadJson(const std::vector<char>& json)
{
  rapidjson::Document* document = GetParsedDocument(json);

  if (document == nullptr) { return 0; }

  std::unordered_map<uint64_t, COCOAnnotation> annotations = ParseAnnotations(document);
}

std::unordered_map<uint64_t, COCOAnnotation> CocoJsonParser::ParseAnnotations(const rapidjson::Document* document)
{
  const rapidjson::Document& doc = *document;

  std::unordered_map<uint64_t, COCOAnnotation> coco_annotations;

  if (document->HasMember("annotations"))
  {
    const rapidjson::Value& annotation_array = doc["annotations"];

    if (!annotation_array.IsArray())
    {
      std::cerr << "Error trying to get annotations array" << std::endl;
      return {};
    }

    for (auto& annotation : annotation_array.GetArray())
    {
      COCOAnnotation coco_annotation;

      coco_annotation.id = ParseValueField<uint64_t>(annotation, "id");
      coco_annotation.categoryId = ParseValueField<uint32_t>(annotation, "category_id");
      coco_annotation.isCrowd = static_cast<bool>(ParseValueField<uint32_t>(annotation, "iscrowd"));
      coco_annotation.imageId = ParseValueField<uint64_t>(annotation, "image_id");

      if (annotation.HasMember("segmentation"))
      {
        const rapidjson::Value& segmentation = annotation["segmentation"];

        if (coco_annotation.isCrowd)
        {
          std::pair<std::vector<float>, std::pair<uint32_t, uint32_t>> result = ParseRLEAnnotationArray(segmentation);
          coco_annotation.segmentation = result.first;
          coco_annotation.size = result.second;
        }
        else
        {
          coco_annotation.segmentation = ParsePolygonAnnotationArray(segmentation);
        }
      }

      coco_annotations[coco_annotation.id] = coco_annotation;
    }
  }

  return coco_annotations;
}

std::pair<std::vector<float>, std::pair<uint32_t, uint32_t>> CocoJsonParser::ParseRLEAnnotationArray(
  const rapidjson::Value& segmentation)
{
  std::vector<float> rle_array;

  if (segmentation.HasMember("counts"))
  {
    const rapidjson::Value& rle_json_array = segmentation["counts"];

    for (auto& rle_value : rle_json_array.GetArray())
    {
      rle_array.emplace_back(rle_value.GetFloat());
    }
  }

  uint32_t width, height;
  if (segmentation.HasMember("size"))
  {
    const rapidjson::Value& rle_json_array = segmentation["size"];

    width = rle_json_array.GetArray()[0].GetInt();
    height = rle_json_array.GetArray()[1].GetInt();
  }

  return { rle_array, { width, height } };
}

std::vector<std::vector<float>> CocoJsonParser::ParsePolygonAnnotationArray(const rapidjson::Value& segmentation)
{
  std::vector<std::vector<float>> polygon_arrays;

  for (auto& polygon_array : segmentation.GetArray())
  {
    polygon_arrays.emplace_back(std::vector<float> {});
    std::vector<float>& last_polygon_array = polygon_arrays.back();

    for (auto& coordinate : polygon_array.GetArray())
    {
      last_polygon_array.emplace_back(coordinate.GetFloat());
    }
  }

  return polygon_arrays;
}

std::unordered_map<uint64_t, COCOImage> CocoJsonParser::ParseImages(const rapidjson::Document& document)
{
  std::unordered_map<uint64_t, COCOImage> coco_images;

  if (document.HasMember("images"))
  {
    const rapidjson::Value& image_array = document["images"];
    assert(image_array.IsArray());

    for (auto& image : image_array.GetArray())
    {
      COCOImage cocoImage;

      cocoImage.id = ParseValueField<uint64_t>(image, "id");
      cocoImage.COCOUrl = ParseValueField<std::string>(image, "coco_url");
      cocoImage.flickrUrl = ParseValueField<std::string>(image, "flickr_url");
      cocoImage.width = ParseValueField<uint32_t>(image, "width");
      cocoImage.height = ParseValueField<uint32_t>(image, "height");
      cocoImage.filename = ParseValueField<std::string>(image, "file_name");

      coco_images[cocoImage.id] = cocoImage;
    }
  }

  return coco_images;
}

std::unordered_map<uint32_t, COCOCategory> CocoJsonParser::ParseCategories(const rapidjson::Document& document)
{
  std::unordered_map<uint32_t, COCOCategory> coco_categories;

  if (document.HasMember("categories"))
  {
    const rapidjson::Value& category_array = document["categories"];
    assert(category_array.IsArray());

    for (auto& category : category_array.GetArray())
    {
      COCOCategory coco_category;

      coco_category.id = ParseValueField<uint32_t>(category, "id");
      coco_category.supercategory = ParseValueField<std::string>(category, "supercategory");
      coco_category.name = ParseValueField<std::string>(category, "name");

      coco_categories[coco_category.id] = coco_category;
    }
  }

  return coco_categories;
}
} // namespace dataset