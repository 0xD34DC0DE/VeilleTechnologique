#include <utility>

#include "asmlgen/application/coco_pipeline/coco_decompression_data.h"

namespace pipeline
{

CocoDecompressionData::CocoDecompressionData(std::vector<AnnotationData> annotation_data,
  const std::vector<std::string>* mask_id_values_name,
  uint32_t image_width,
  uint32_t image_height,
  std::string filename,
  std::string filepath)
  : annotations_data_(std::move(annotation_data)), mask_id_values_name_(mask_id_values_name), image_width_(image_width),
    image_height_(image_height), filename_(std::move(filename)), filepath_(std::move(filepath)), is_valid_(false)
{}

const std::vector<AnnotationData>& CocoDecompressionData::GetAnnotationsData() const noexcept
{
  return annotations_data_;
}

void CocoDecompressionData::SetAnnotationsData(std::vector<AnnotationData> annotations_data) noexcept
{
  annotations_data_ = std::move(annotations_data);
}

uint32_t CocoDecompressionData::GetImageWidth() const noexcept
{
  return image_width_;
}

void CocoDecompressionData::SetImageWidth(uint32_t image_width) noexcept
{
  image_width_ = image_width;
}

uint32_t CocoDecompressionData::GetImageHeight() const noexcept
{
  return image_height_;
}

void CocoDecompressionData::SetImageHeight(uint32_t image_height) noexcept
{
  image_height_ = image_height;
}

const std::string& CocoDecompressionData::GetFilename() const noexcept
{
  return filename_;
}

void CocoDecompressionData::SetFilename(const std::string& filename)
{
  filename_ = filename;
}

const std::vector<std::string>* CocoDecompressionData::GetMaskIdValuesName() const noexcept
{
  return mask_id_values_name_;
}

void CocoDecompressionData::SetMaskIdValuesName(const std::vector<std::string>* mask_id_values_name) noexcept
{
  mask_id_values_name_ = mask_id_values_name;
}

const std::vector<std::pair<std::string, std::vector<uint8_t>>>& CocoDecompressionData::GetMasksBytes() const noexcept
{
  return masks_bytes_;
}

void CocoDecompressionData::SetMaskBytes(
  std::vector<std::pair<std::string, std::vector<uint8_t>>>&& decompressed_mask_bytes)
{
  masks_bytes_ = std::move(decompressed_mask_bytes);
}

void CocoDecompressionData::SetValid(bool valid) noexcept
{
  is_valid_ = valid;
}

bool CocoDecompressionData::IsValid() const noexcept
{
  return is_valid_;
}
const std::string& CocoDecompressionData::GetFilepath() const
{
  return filepath_;
}

void CocoDecompressionData::SetFilepath(const std::string& filepath)
{
  filepath_ = filepath;
}

std::vector<std::pair<std::string, std::vector<uint8_t>>>& CocoDecompressionData::GetMasksBytes() noexcept
{
  return masks_bytes_;
}

} // namespace pipeline
