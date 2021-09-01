#include "asmlgen/application/dataset/transient_dataset_entry.h"

namespace dataset
{

uint32_t TransientDatasetEntry::GetImageWidth() const noexcept
{
  return image_width_;
}

uint32_t TransientDatasetEntry::GetImageHeight() const noexcept
{
  return image_height_;
}

const std::vector<uint8_t>& TransientDatasetEntry::GetImageBytes() const noexcept
{
  return image_bytes_;
}

const std::vector<uint8_t>& TransientDatasetEntry::GetMaskBytes() const noexcept
{
  return mask_bytes_;
}

const std::vector<std::string>& TransientDatasetEntry::GetMaskIdValuesName() const noexcept
{
  return _mask_id_values_name;
}

} // namespace dataset