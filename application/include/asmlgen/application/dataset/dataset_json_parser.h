#ifndef ASMLGEN_DATASET_JSON_PARSER_H
#define ASMLGEN_DATASET_JSON_PARSER_H

#include <unordered_map>
#include <vector>

namespace dataset
{

// TODO replace T by concept that defines a transient entry (a pre-entry that is not downloaded or compiled yet and has
// so general functions like GetCategory, GetClass, GetSize, AcquireData...)
template<typename T>
class DatasetJsonParser
{
public:
  using ClassNameGroupMap = std::unordered_map<std::string, std::vector<std::string>>;

  DatasetJsonParser() = default;

  // Takes in a char vector representing the json file and returns the number of elements processed
  // Needs to be implemented for a given dataset
  virtual uint64_t LoadJson(const std::vector<char>& json) = 0;

  // Get the processed items
  [[nodiscard]] const std::vector<T>& Entries() const noexcept;

  // Get the map of category names grouped into classes
  [[nodiscard]] const ClassNameGroupMap& ClassNameGroups() const noexcept;

private:
  std::vector<T> entries_;
  std::unordered_map<std::string, std::vector<std::string>> class_name_groups_;
};

} // namespace dataset
#endif // ASMLGEN_DATASET_JSON_PARSER_H
