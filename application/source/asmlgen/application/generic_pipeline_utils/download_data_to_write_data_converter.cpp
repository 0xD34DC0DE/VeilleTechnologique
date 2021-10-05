#include "asmlgen/application/generic_pipeline_utils/download_data_to_write_data_converter.h"
#include <iostream>

namespace pipeline
{

FileWriteData* DownloadDataToWriteDataConverter::Call(DownloadData* download_data)
{
  if (download_data->HasFailed())
  {
    std::cerr << "nope: " << download_data->GetUrl() << std::endl;
    return {};
  };
  std::vector<std::vector<uint8_t>> data = { { std::move(download_data->GetDataRvalRef()) } };
  std::string filename = download_data->GetResource().substr(
    download_data->GetResource().find_last_of('/') + 1, download_data->GetResource().size());
  file_write_data_ = { download_data->GetOutputFilepaths(), std::move(filename), std::move(data) };
  return &file_write_data_;
}

} // namespace pipeline