#include "asmlgen/application/coco_pipeline/encode_image.h"
#include <iterator>

//#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace pipeline
{

CocoDecompressionData* EncodeImage::Call(CocoDecompressionData* decompression_data)
{
  if (decompression_data->IsValid())
  {
    std::vector<std::pair<std::string, std::vector<uint8_t>>> masks;
    for (const auto& mask : decompression_data->GetMasksBytes())
    {
      std::vector<uint8_t> data;
      int32_t width = decompression_data->GetImageWidth();
      int32_t height = decompression_data->GetImageHeight();

      stbi_write_jpg_to_func(
        [](void* context, void* data, int size)
        {
          auto* buffer = static_cast<uint8_t*>(data);
          auto result_buffer = static_cast<std::vector<uint8_t>*>(context);
          result_buffer->insert(result_buffer->end(), buffer, buffer + size);
        },
        &data,
        width,
        height,
        3,
        mask.second.data(),
        85);
      masks.emplace_back(mask.first, std::move(data));
    }
    decompression_data->SetMaskBytes(std::move(masks));
  }
  return decompression_data;
}

} // namespace pipeline
