#ifndef ASMLGEN_MASK_DECOMPRESSION_H
#define ASMLGEN_MASK_DECOMPRESSION_H

#include "asmlgen/application/coco_pipeline/coco_decompression_data.h"
#include "asmlgen/application/tasking/call_chain.h"

#include <vector>

namespace pipeline
{

class MaskDecompression
  : public tasking::MakeChainObj<MaskDecompression, CocoDecompressionData*, CocoDecompressionData*>
{
public:
  CocoDecompressionData* Call(CocoDecompressionData* coco_decompression_data);

private:
  struct Edge;

  std::vector<uint8_t> DecompressAnnotationToMask(
    unsigned int width, unsigned int height, const SegmentationData& segmentation);

  std::vector<uint8_t> ScanlineFill(unsigned int width, unsigned int height, std::vector<Edge>& edges);

  std::vector<uint8_t> RleFill(unsigned int width, unsigned int height, const std::vector<float>& rle);

  std::vector<Edge> GenerateEdges(const SegmentationData& segmentationData);
};

} // namespace pipeline

#endif
