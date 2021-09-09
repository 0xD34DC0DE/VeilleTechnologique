#ifndef ASMLGEN_TRANSIENT_COCO_ENTRY_H
#define ASMLGEN_TRANSIENT_COCO_ENTRY_H

#include "asmlgen/application/dataset/coco_dataset_types.h"
#include "asmlgen/application/dataset/transient_dataset_entry.h"

#include <cstdint>
#include <string>
#include <vector>

namespace dataset
{

class TransientCocoEntry : public TransientDatasetEntry
{
public:
  friend class CocoJsonParser;
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

protected:
  // Coco dataset format implementation types and function used by friend parser class
  uint32_t image_width_ {};
  uint32_t image_height_ {};
  std::string image_url_;

  // List of annotations and their corresponding category name
  std::vector<std::pair<std::string, SegmentationData>> annotation_segmentations_data;
};

} // namespace dataset

#endif
