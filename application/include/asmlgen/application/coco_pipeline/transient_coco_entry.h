#ifndef ASMLGEN_TRANSIENT_COCO_ENTRY_H
#define ASMLGEN_TRANSIENT_COCO_ENTRY_H

#include "asmlgen/application/coco_pipeline/coco_dataset_types.h"
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

  [[nodiscard]] std::string GetURL() const
  {
    return image_url_;
  }

  [[nodiscard]] std::vector<AnnotationData> AnnotationsData() const noexcept
  {
    return annotation_segmentations_data;
  }

  std::string GetWritePath() const noexcept;

protected:
  // Coco dataset format implementation types used by friend parser class
  std::string image_url_;
  std::string write_path_;

  // List of annotations and their corresponding category name
  std::vector<AnnotationData> annotation_segmentations_data;
};

} // namespace dataset

#endif
