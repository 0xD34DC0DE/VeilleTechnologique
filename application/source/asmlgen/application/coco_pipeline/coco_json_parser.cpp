#include "asmlgen/application/coco_pipeline/coco_json_parser.h"

#include "asmlgen/application/dataset/json_parsing_helpers.h"

#include <algorithm>

namespace dataset
{

uint64_t CocoJsonParser::LoadJson(const std::vector<char>& json)
{
  rapidjson::Document* document = GetParsedDocument(json);

  if (document == nullptr) { return 0; }

  AnnotationMap annotations = ParseAnnotations(document);
  ImageMap images = ParseImages(document);
  CategoryMap categories = ParseCategories(document);

  LinkImagesToAnnotations(annotations, images);
  LinkCategoriesToAnnotations(annotations, categories);

  class_name_groups_ = CreateClassNameGroups(categories);

  const std::unordered_map<uint64_t, TransientCocoEntry> transient_entries = GenerateTransientEntries(annotations);

  for (const auto& id_transient_entry_pair : transient_entries)
  {
    entries_.emplace_back(id_transient_entry_pair.second);
  }

  return entries_.size();
}

std::unordered_map<uint64_t, TransientCocoEntry> CocoJsonParser::GenerateTransientEntries(
  const CocoJsonParser::AnnotationMap& annotations)
{
  std::unordered_map<uint64_t, TransientCocoEntry> transient_entries;
  for (const auto& id_annotation_pair : annotations)
  {
    auto transient_entry_iter = transient_entries.find(id_annotation_pair.second.imageId);
    if (transient_entry_iter == transient_entries.end())
    {
      TransientCocoEntry transient_coco_entry = TransientCocoEntry();
      const COCOImage* coco_image = id_annotation_pair.second.cocoImagePtr;

      transient_coco_entry.image_width_ = coco_image->width;
      transient_coco_entry.image_height_ = coco_image->height;
      transient_coco_entry.image_url_ = coco_image->COCOUrl;

      transient_coco_entry.annotation_segmentations_data.emplace_back(
        id_annotation_pair.second.category_name, id_annotation_pair.second.segmentation);

      transient_entries[id_annotation_pair.second.imageId] = transient_coco_entry;
    }
    else
    {
      transient_entry_iter->second.annotation_segmentations_data.emplace_back(
        id_annotation_pair.second.category_name, id_annotation_pair.second.segmentation);
    }
  }

  return transient_entries;
}

CocoJsonParser::AnnotationMap CocoJsonParser::ParseAnnotations(const rapidjson::Document* document)
{
  const rapidjson::Document& doc = *document;
  AnnotationMap coco_annotations;

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

CocoJsonParser::ImageMap CocoJsonParser::ParseImages(const rapidjson::Document* document)
{
  const rapidjson::Document& doc = *document;
  ImageMap coco_images;

  if (doc.HasMember("images"))
  {
    const rapidjson::Value& image_array = doc["images"];
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

CocoJsonParser::CategoryMap CocoJsonParser::ParseCategories(const rapidjson::Document* document)
{
  const rapidjson::Document& doc = *document;
  CategoryMap coco_categories;

  if (doc.HasMember("categories"))
  {
    const rapidjson::Value& category_array = doc["categories"];
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

void CocoJsonParser::LinkImagesToAnnotations(
  CocoJsonParser::AnnotationMap& annotations, const CocoJsonParser::ImageMap& images)
{
  for (auto& annotation : annotations)
  {
    auto imageIt = images.find(annotation.second.imageId);
    if (imageIt != images.end()) { annotation.second.cocoImagePtr = &imageIt->second; }
  }
}

void CocoJsonParser::LinkCategoriesToAnnotations(
  CocoJsonParser::AnnotationMap& annotations, const CocoJsonParser::CategoryMap& categories)
{
  for (auto& annotation : annotations)
  {
    auto categoryIt = categories.find(annotation.second.categoryId);
    if (categoryIt != categories.end()) { annotation.second.category_name = categoryIt->second.name; }
  }
}

DatasetJsonParser<CocoJsonParser>::ClassNameGroupMap CocoJsonParser::CreateClassNameGroups(
  const CategoryMap& categories)
{
  ClassNameGroupMap class_name_group_map;

  for (auto& category : categories)
  {
    auto classIt = class_name_group_map.find(category.second.supercategory);
    if (classIt != class_name_group_map.end())
    {
      auto name_iter = std::ranges::find(classIt->second, category.second.name);
      if (name_iter == classIt->second.end()) { classIt->second.emplace_back(category.second.name); }
    }
    else
    {
      class_name_group_map[category.second.supercategory] = { category.second.name };
    }
  }

  return class_name_group_map;
}
} // namespace dataset