#include "asmlgen/application/dataset/dataset_entry.h"

#include "asmlgen/application/dataset/dataset_entry_id_sequence.h"

namespace dataset
{

DatasetEntry::DatasetEntry() : id_(DatasetEntryIdSequence::Next()), image_width_(0), image_height_(0) {}

uint64_t DatasetEntry::GetId() const noexcept
{
  return id_;
}

uint32_t DatasetEntry::GetImageWidth() const noexcept
{
  return image_width_;
}

uint32_t DatasetEntry::GetImageHeight() const noexcept
{
  return image_height_;
}

const std::vector<uint8_t>& DatasetEntry::GetImageBytes() const noexcept
{
  return image_bytes_;
}

const std::vector<uint8_t>& DatasetEntry::GetMaskBytes() const noexcept
{
  return mask_bytes_;
}

const std::vector<std::string>& DatasetEntry::GetMaskIdValuesName() const noexcept
{
  return _mask_id_values_name;
}

void DatasetEntry::SetImageSize(uint32_t image_width, uint32_t image_height) noexcept
{
  image_width_ = image_width;
  image_height_ = image_height;
}

void DatasetEntry::SetImageBytes(const std::vector<uint8_t>& image_bytes) noexcept
{
  image_bytes_ = image_bytes;
}

void DatasetEntry::SetMaskBytes(const std::vector<uint8_t>& mask_bytes) noexcept
{
  mask_bytes_ = mask_bytes;
}

void DatasetEntry::SetMaskIdValuesName(const std::vector<std::string>& mask_id_values_name) noexcept
{
  _mask_id_values_name = mask_id_values_name;
}

} // namespace dataset