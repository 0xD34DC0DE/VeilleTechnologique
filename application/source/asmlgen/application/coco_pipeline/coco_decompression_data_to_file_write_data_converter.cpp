#include "asmlgen/application/coco_pipeline/coco_decompression_data_to_file_write_data_converter.h"

namespace pipeline
{

FileWriteData* CocoDecompressionDataToFileWriteDataConverter::Call(CocoDecompressionData* decompression_data)
{
  if (decompression_data->IsValid())
  {
    std::vector<std::string> file_paths;
    std::vector<std::vector<uint8_t>> masks_bytes;
    for (auto& name_mask_byte_pair : decompression_data->GetMasksBytes())
    {
      file_paths.emplace_back(
        decompression_data->GetFilepath() + "/" + name_mask_byte_pair.first + "/" + decompression_data->GetFilename());
      masks_bytes.emplace_back(std::move(name_mask_byte_pair.second));
    }

    file_write_data_ = FileWriteData(file_paths, decompression_data->GetFilename(), masks_bytes, true);
    return &file_write_data_;
  }
  else
  {
    return {};
  }
}

} // namespace pipeline
