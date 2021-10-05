#ifndef ASMLGEN_COCOTRANSIENTENTRYTOCOCODECOMPRESSIONDATACONVERTER_H
#define ASMLGEN_COCOTRANSIENTENTRYTOCOCODECOMPRESSIONDATACONVERTER_H

#include "asmlgen/application/coco_pipeline/coco_decompression_data.h"
#include "asmlgen/application/coco_pipeline/transient_coco_entry.h"
#include "asmlgen/application/tasking/call_chain.h"

namespace pipeline
{

class CocoTransientEntryToCocoDecompressionDataConverter
  : public tasking::
      MakeChainObj<CocoTransientEntryToCocoDecompressionDataConverter, CocoDecompressionData*, TransientCocoEntry*>
{
public:
  CocoDecompressionData* Call(TransientCocoEntry* transient_coco_entry);

private:
  CocoDecompressionData coco_decompression_data_;
};

} // namespace pipeline
#endif
