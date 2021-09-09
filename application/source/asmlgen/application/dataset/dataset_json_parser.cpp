#include "asmlgen/application/dataset/dataset_json_parser.h"

namespace dataset
{

template<typename T>
const std::vector<TransientDatasetEntry>& DatasetJsonParser<T>::Entries() const noexcept
{
  return static_cast<std::vector<TransientDatasetEntry>&>(entries_);
}

template<typename T>
const std::unordered_map<std::string, std::vector<std::string>>& DatasetJsonParser<T>::ClassNameGroups() const noexcept
{
  return class_name_groups_;
}

template<typename T>
uint64_t DatasetJsonParser<T>::LoadJson(const std::vector<char>&)
{
  return 0;
}

} // namespace dataset