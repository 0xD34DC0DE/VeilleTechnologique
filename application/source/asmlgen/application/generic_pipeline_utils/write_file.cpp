#include "asmlgen/application/generic_pipeline_utils/write_file.h"

#include "asmlgen/application/config/assertion.h"

#include <filesystem>
#include <fstream>

namespace pipeline
{

std::size_t WriteFile::Call(FileWriteData* file_write_data) const
{
  if (!file_write_data->GetData().empty() && file_write_data->GetDataSize() > 0)
  {
    std::string fullpath = file_write_data->GetFullPath();
    if (!std::filesystem::exists(fullpath))
    {
      std::ofstream output_file_stream(fullpath, std::ios::out | std::ios::binary);
      if (!output_file_stream) { ASSERT(false, std::string("Could not write file to path: ").append(fullpath)); }

      output_file_stream.write(
        reinterpret_cast<const char*>(file_write_data->GetDataPtr()), file_write_data->GetDataSize());
      return file_write_data->GetDataSize();
    }
  }

  return 0; // no data written
}
} // namespace pipeline