#include "asmlgen/application/generic_pipeline_utils/download_data_to_write_data_converter.h"

namespace pipeline
{

FileWriteData* DownloadDataToWriteDataConverter::Call(DownloadData* download_data)
{
  if (download_data->HasFailed()) return {};

  std::string filename = download_data->GetResource().substr(
    download_data->GetResource().find_last_of('/'), download_data->GetResource().size());
  file_write_data_ = {
    download_data->GetOutputFilepath(), std::move(filename), std::move(download_data->GetDataRvalRef())
  };
  return &file_write_data_;
}

} // namespace pipeline