#include "asmlgen/application/dataset/dataset_entry_id_sequence.h"

namespace dataset
{

uint64_t DatasetEntryIdSequence::Next() noexcept
{
  constinit static std::atomic_uint64_t next = 0;
  return next++;
}

} // namespace dataset