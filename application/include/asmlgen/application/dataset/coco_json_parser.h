#ifndef ASMLGEN_COCO_JSON_PARSER_H
#define ASMLGEN_COCO_JSON_PARSER_H

#include "asmlgen/application/dataset/dataset_json_parser.h"

#include <unordered_map>

namespace dataset
{
class CocoJsonParser : public DatasetJsonParser<int>
{
public:
  CocoJsonParser() = default;

  uint64_t LoadJson(const std::vector<char>& json) override;

private:
  //  using AnnotationMap = std::unordered_map<uint64_t, COCOAnnotation>;
  //  using ImageMap = std::unordered_map<uint64_t, COCOImage>;
  //  using CategoryMap = std::unordered_map<uint32_t, COCOCategory>;
  //
  //  AnnotationMap annotations_;
  //  ImageMap images_;
  //  CategoryMap categories_;
};
} // namespace dataset

#endif
