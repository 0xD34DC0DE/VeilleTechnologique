#ifndef ASMLGEN_DOWNLOAD_DATA_TO_WRITE_DATA_CONVERTER_H
#define ASMLGEN_DOWNLOAD_DATA_TO_WRITE_DATA_CONVERTER_H

#include "asmlgen/application/generic_pipeline_utils/download_data.h"
#include "asmlgen/application/generic_pipeline_utils/file_write_data.h"
#include "asmlgen/application/tasking/call_chain.h"

namespace pipeline
{

class DownloadDataToWriteDataConverter
  : public tasking::MakeChainObj<DownloadDataToWriteDataConverter, FileWriteData*, DownloadData*>
{
public:
  FileWriteData* Call(DownloadData* download_data);

private:
  FileWriteData file_write_data_;
};

} // namespace pipeline
#endif
