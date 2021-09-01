#ifndef ASMLGEN_DATASET_ENTRY_H
#define ASMLGEN_DATASET_ENTRY_H

#include <cstdint>
#include <string>
#include <vector>

namespace dataset
{

class DatasetEntry
{
public:
  DatasetEntry();

  [[nodiscard]] uint64_t GetId() const noexcept;
  [[nodiscard]] uint32_t GetImageWidth() const noexcept;
  [[nodiscard]] uint32_t GetImageHeight() const noexcept;

  [[nodiscard]] const std::vector<uint8_t>& GetImageBytes() const noexcept;
  [[nodiscard]] const std::vector<uint8_t>& GetMaskBytes() const noexcept;
  [[nodiscard]] const std::vector<std::string>& GetMaskIdValuesName() const noexcept;

  void SetImageSize(uint32_t image_width, uint32_t image_height) noexcept;
  void SetImageBytes(const std::vector<uint8_t>& image_bytes) noexcept;
  void SetMaskBytes(const std::vector<uint8_t>& mask_bytes) noexcept;
  void SetMaskIdValuesName(const std::vector<std::string>& mask_id_values_name) noexcept;

private:
  uint64_t id_;
  uint32_t image_width_;
  uint32_t image_height_;

  std::vector<uint8_t> image_bytes_;
  std::vector<uint8_t> mask_bytes_;

  /// Used to map the id's stored in the mask bytes to their corresponding class name
  // index 0 is always "None"
  std::vector<std::string> _mask_id_values_name;
};

} // namespace dataset

#endif // ASMLGEN_DATASET_ENTRY_H
