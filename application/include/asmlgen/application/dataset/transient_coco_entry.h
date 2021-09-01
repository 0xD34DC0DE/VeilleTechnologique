#ifndef ASMLGEN_TRANSIENT_COCO_ENTRY_H
#define ASMLGEN_TRANSIENT_COCO_ENTRY_H

#include "transient_dataset_entry.h"

namespace dataset
{

class TransientCocoEntry : public TransientDatasetEntry
{
public:
  TransientCocoEntry() = default;

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
};

} // namespace dataset

#endif
