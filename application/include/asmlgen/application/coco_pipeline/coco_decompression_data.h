#ifndef ASMLGEN_COCODECOMPRESSIONDATA_H
#define ASMLGEN_COCODECOMPRESSIONDATA_H

#include "asmlgen/application/coco_pipeline/coco_dataset_types.h"

namespace pipeline
{

using namespace dataset;

class CocoDecompressionData
{
public:
  CocoDecompressionData() = default;
  CocoDecompressionData(std::vector<AnnotationData> segmentation_data,
    const std::vector<std::string>* mask_id_values_name,
    uint32_t image_width,
    uint32_t image_height,
    std::string filename,
    std::string filepath);

  void SetAnnotationsData(std::vector<AnnotationData> annotations_data) noexcept;

  void SetImageWidth(uint32_t image_width) noexcept;

  void SetImageHeight(uint32_t image_height) noexcept;

  void SetFilename(const std::string& filename);

  void SetFilepath(const std::string& filepath);

  void SetMaskIdValuesName(const std::vector<std::string>* mask_id_values_name) noexcept;

  void SetMaskBytes(std::vector<std::pair<std::string, std::vector<uint8_t>>>&& decompressed_mask_bytes);

  void SetValid(bool valid) noexcept;

  [[nodiscard]] const std::vector<AnnotationData>& GetAnnotationsData() const noexcept;

  [[nodiscard]] uint32_t GetImageWidth() const noexcept;

  [[nodiscard]] uint32_t GetImageHeight() const noexcept;

  [[nodiscard]] const std::string& GetFilename() const noexcept;

  [[nodiscard]] const std::string& GetFilepath() const;

  [[nodiscard]] const std::vector<std::string>* GetMaskIdValuesName() const noexcept;

  [[nodiscard]] const std::vector<std::pair<std::string, std::vector<uint8_t>>>& GetMasksBytes() const noexcept;

  std::vector<std::pair<std::string, std::vector<uint8_t>>>& GetMasksBytes() noexcept;

  [[nodiscard]] bool IsValid() const noexcept;

private:
  std::vector<AnnotationData> annotations_data_;
  uint32_t image_width_;
  uint32_t image_height_;
  std::string filename_;
  std::string filepath_;

  bool is_valid_;

  const std::vector<std::string>* mask_id_values_name_;
  std::vector<std::pair<std::string, std::vector<uint8_t>>> masks_bytes_;
};

} // namespace pipeline

#endif
