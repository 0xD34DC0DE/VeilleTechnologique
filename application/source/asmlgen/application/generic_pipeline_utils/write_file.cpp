#include "asmlgen/application/generic_pipeline_utils/write_file.h"

#include "asmlgen/application/config/assertion.h"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace pipeline
{

std::size_t WriteFile::Call(FileWriteData* file_write_data) const
{
  if (!file_write_data || file_write_data->GetDatas().size() == 0) { return 0; }

  size_t size_sum = 0;
  size_t file_count = file_write_data->GetDatas().size();
  if (!file_write_data->IsDecompression())
  {
    const std::vector<uint8_t>& data = file_write_data->GetDatas()[0];
    for (const auto& file_path : file_write_data->GetDirectoryPaths())
    {
      std::string full_path = file_path + file_write_data->GetFilename();

      if (!std::filesystem::exists(file_path)) { std::filesystem::create_directory(file_path); }

      if (!std::filesystem::exists(full_path))
      {
        std::ofstream output_file_stream(full_path, std::ios::out | std::ios::binary);
        if (!output_file_stream) { ASSERT(false, std::string("Could not write file to path: ").append(full_path)); }

        output_file_stream.write(reinterpret_cast<const char*>(data.data()), data.size());
        size_sum += data.size();
      }
    }
  }
  else
  {
    for (uint32_t i = 0; i < file_write_data->GetDatas().size(); ++i)
    {
      const std::vector<uint8_t>& data = file_write_data->GetDatas()[i];
      const std::string& file_path = file_write_data->GetDirectoryPaths()[i];
      const std::string directory = file_path.substr(0, file_path.find_last_of('/') + 1);

      if (!std::filesystem::exists(directory)) { std::filesystem::create_directory(directory); }

      if (!std::filesystem::exists(file_path))
      {
        std::ofstream output_file_stream(file_path, std::ios::out | std::ios::binary);
        if (!output_file_stream) { ASSERT(false, std::string("Could not write file to path: ").append(file_path)); }

        output_file_stream.write(reinterpret_cast<const char*>(data.data()), data.size());
        size_sum += data.size();
      }
    }
  }

  return size_sum; // no data written
}
} // namespace pipeline