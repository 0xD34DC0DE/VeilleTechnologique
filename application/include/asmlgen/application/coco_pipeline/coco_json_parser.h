#ifndef ASMLGEN_COCO_JSON_PARSER_H
#define ASMLGEN_COCO_JSON_PARSER_H

#include "asmlgen/application/coco_pipeline/coco_dataset_types.h"
#include "asmlgen/application/coco_pipeline/transient_coco_entry.h"
#include "asmlgen/application/dataset/dataset_json_parser.h"
#include "asmlgen/application/dataset/transient_dataset_entry.h"

#include "rapidjson/document.h"

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace dataset
{

class CocoJsonParser : public DatasetJsonParser<TransientCocoEntry>
{
public:
  CocoJsonParser() = default;
  ~CocoJsonParser() override = default;

  uint64_t LoadJson(
    const std::vector<char>& json, const std::vector<std::string>& classes, const std::string& write_path) override;

  [[nodiscard]] const std::vector<std::string>& GetMaskIdValuesName() const noexcept;

private:
  using AnnotationMap = std::unordered_map<uint64_t, COCOAnnotation>;
  using ImageMap = std::unordered_map<uint64_t, COCOImage>;
  using CategoryMap = std::unordered_map<uint32_t, COCOCategory>;

  /// Used to map the id's stored in the mask bytes to their corresponding class name
  // index 0 is always "None"
  std::vector<std::string> mask_id_values_name_;

  static std::unordered_map<uint64_t, COCOAnnotation> ParseAnnotations(const rapidjson::Document* document);

  static std::pair<std::vector<float>, std::pair<uint32_t, uint32_t>> ParseRLEAnnotationArray(
    const rapidjson::Value& segmentation);

  static std::vector<std::vector<float>> ParsePolygonAnnotationArray(const rapidjson::Value& segmentation);

  static std::unordered_map<uint64_t, COCOImage> ParseImages(const rapidjson::Document* document);

  static std::unordered_map<uint32_t, COCOCategory> ParseCategories(const rapidjson::Document* document);

  static void LinkImagesToAnnotations(AnnotationMap& annotations, const ImageMap& images);

  static void LinkCategoriesToAnnotations(AnnotationMap& annotations, const CategoryMap& categories);

  static DatasetJsonParser<TransientCocoEntry>::ClassNameGroupMap CreateClassNameGroups(const CategoryMap& categories);

  static std::unordered_map<uint64_t, TransientCocoEntry> GenerateTransientEntries(const AnnotationMap& annotations,
    const std::vector<std::string>* mask_id_values_name,
    const std::string& write_path,
    const std::vector<std::string>& classes);

  static std::vector<std::string> GenerateMaskIdValueNames(const CategoryMap& categories);
};

} // namespace dataset

#endif
