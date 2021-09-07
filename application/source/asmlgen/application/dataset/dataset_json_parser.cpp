#include "asmlgen/application/dataset/dataset_json_parser.h"

namespace dataset
{

template<typename T>
const std::vector<T>& DatasetJsonParser<T>::Entries() const noexcept
{
  return entries_;
}

template<typename T>
const std::unordered_map<std::string, std::vector<std::string>>& DatasetJsonParser<T>::ClassNameGroups() const noexcept
{
  return class_name_groups_;
}

} // namespace dataset