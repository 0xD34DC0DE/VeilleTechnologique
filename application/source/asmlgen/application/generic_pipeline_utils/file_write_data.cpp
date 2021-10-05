#include <utility>

#include "asmlgen/application/generic_pipeline_utils/file_write_data.h"

namespace pipeline
{

FileWriteData::FileWriteData(std::vector<std::string> directory_paths,
  std::string filename,
  std::vector<std::vector<uint8_t>> datas,
  bool is_decompression)
  : directory_paths_(std::move(directory_paths)), filename_(std::move(filename)), datas_(std::move(datas)),
    is_decompression_(is_decompression)
{
  if (!is_decompression)
  {
    for (auto& directory_path : directory_paths_)
    {
      if (!directory_path.empty())
      {
        const char& last_char = directory_path.back();
        if (last_char != '/' || last_char != '\\') { directory_path.append("/"); }
      }
    }
  }
}

const std::vector<std::string>& FileWriteData::GetDirectoryPaths() const noexcept
{
  return directory_paths_;
}

const std::string& FileWriteData::GetFilename() const noexcept
{
  return filename_;
}

const std::vector<std::vector<uint8_t>>& FileWriteData::GetDatas() const noexcept
{
  return datas_;
}

bool FileWriteData::IsDecompression() const
{
  return is_decompression_;
}

} // namespace pipeline