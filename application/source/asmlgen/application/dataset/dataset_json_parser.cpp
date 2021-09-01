#include "asmlgen/application/dataset/dataset_json_parser.h"

namespace dataset
{

template<typename T>
const std::vector<T>& DatasetJsonParser<T>::Entries() const noexcept
{
  return entries_;
}

} // namespace dataset