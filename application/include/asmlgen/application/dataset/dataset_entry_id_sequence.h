#ifndef ASMLGEN_DATASET_ENTRY_ID_SEQUENCE_H
#define ASMLGEN_DATASET_ENTRY_ID_SEQUENCE_H

#include <atomic> //std::atomic_uint32_t
#include <cstdint>

namespace dataset
{

class DatasetEntryIdSequence
{
public:
  [[nodiscard]] static uint64_t Next() noexcept;
};

} // namespace dataset

#endif // ASMLGEN_DATASET_ENTRY_ID_SEQUENCE_H
