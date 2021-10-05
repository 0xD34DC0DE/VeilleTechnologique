#ifndef ASMLGEN_COCO_DECOMPRESSION_DATA_TO_FILE_WRITE_DATA_CONVERTER_H
#define ASMLGEN_COCO_DECOMPRESSION_DATA_TO_FILE_WRITE_DATA_CONVERTER_H

#include "asmlgen/application/coco_pipeline/coco_decompression_data.h"
#include "asmlgen/application/generic_pipeline_utils/file_write_data.h"
#include "asmlgen/application/tasking/call_chain.h"

namespace pipeline
{

class CocoDecompressionDataToFileWriteDataConverter
  : public tasking::MakeChainObj<CocoDecompressionDataToFileWriteDataConverter, FileWriteData*, CocoDecompressionData*>
{
public:
  FileWriteData* Call(CocoDecompressionData* decompression_data);

private:
  FileWriteData file_write_data_;
};

} // namespace pipeline

#endif
