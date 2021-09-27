#include <utility>

#include "asmlgen/application/generic_pipeline_utils/file_write_data.h"

namespace pipeline
{

FileWriteData::FileWriteData(std::string directory_path, std::string filename, std::vector<uint8_t> data)
  : directory_path_(std::move(directory_path)), filename_(std::move(filename)), data_(std::move(data))
{
  if (!directory_path_.empty())
  {
    const char& last_char = directory_path_.back();
    if (last_char != '/' || last_char != '\\') { directory_path_.append("/"); }
  }
}

const std::string& FileWriteData::GetDirectoryPath() const noexcept
{
  return directory_path_;
}

const std::string& FileWriteData::GetFilename() const noexcept
{
  return filename_;
}

const std::vector<uint8_t>& FileWriteData::GetData() const noexcept
{
  return data_;
}

std::string FileWriteData::GetFullPath() const
{
  return directory_path_ + filename_;
}

const uint8_t* FileWriteData::GetDataPtr() const noexcept
{
  return data_.data();
}

std::size_t FileWriteData::GetDataSize() const noexcept
{
  return data_.size();
}

} // namespace pipeline