#ifndef ASMLGEN_TRANSIENT_DATASET_ENTRY_H
#define ASMLGEN_TRANSIENT_DATASET_ENTRY_H

#include <cstdint>
#include <string>
#include <vector>

namespace dataset
{
// Transient entry is used as a midpoint between a DatasetEntry and raw values from the processed json file.
// The transient entry class is to be extended with processing functions to fit the data in a common representation
// (DatasetEntry).
// Also, this class is very similar to the DatasetEntry class but uses a virtual function whereas the DatasetEntry
// doesn't that means that the DatasetEntry class can benefit in performance by not using the v-table
class TransientDatasetEntry
{
public:
  TransientDatasetEntry() = default;

  [[nodiscard]] uint32_t GetImageWidth() const noexcept;
  [[nodiscard]] uint32_t GetImageHeight() const noexcept;

  [[nodiscard]] const std::vector<uint8_t>& GetImageBytes() const noexcept;
  [[nodiscard]] const std::vector<uint8_t>& GetMaskBytes() const noexcept;
  [[nodiscard]] const std::vector<std::string>& GetMaskIdValuesName() const noexcept;

  ///
  /// Retrieve the data if there is a need to. Example: downloading, query a local database
  ///
  /// @return size_t Number of bytes acquired
  virtual std::size_t AcquireData() = 0;

  ///
  /// Process the data to fit the common representation (DatasetEntry)
  ///
  virtual void ProcessData() = 0;

private:
  uint32_t image_width_ = 0;
  uint32_t image_height_ = 0;

  std::vector<uint8_t> image_bytes_;
  std::vector<uint8_t> mask_bytes_;

  /// Used to map the id's stored in the mask bytes to their corresponding class name
  // index 0 is always "None"
  std::vector<std::string> _mask_id_values_name;
};

} // namespace dataset

#endif
