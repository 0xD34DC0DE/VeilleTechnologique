#ifndef ASMLGEN_TRANSIENT_COCO_ENTRY_H
#define ASMLGEN_TRANSIENT_COCO_ENTRY_H

#include "asmlgen/application/dataset/coco_dataset_types.h"
#include "asmlgen/application/dataset/transient_dataset_entry.h"

namespace dataset
{

class TransientCocoEntry : public TransientDatasetEntry
{
public:
  TransientCocoEntry() = default;
  ~TransientCocoEntry() override = default;

  ///
  /// Retrieve the data if there is a need to. Example: downloading, query a local database
  ///
  /// @return size_t Number of bytes acquired
  std::size_t AcquireData() override;

  ///
  /// Process the data to fit the common representation (DatasetEntry)
  ///
  void ProcessData() override;

private:
  // Coco dataset format implementation types and function used by friend parser class
};

} // namespace dataset

#endif
