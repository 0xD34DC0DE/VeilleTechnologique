#ifndef ASMLGEN_ENCODE_IMAGE_H
#define ASMLGEN_ENCODE_IMAGE_H

#include "asmlgen/application/coco_pipeline/coco_decompression_data.h"
#include "asmlgen/application/tasking/call_chain.h"

namespace pipeline
{

class EncodeImage : public tasking::MakeChainObj<EncodeImage, CocoDecompressionData*, CocoDecompressionData*>
{
public:
  CocoDecompressionData* Call(CocoDecompressionData* decompression_data);
};

} // namespace pipeline
#endif
