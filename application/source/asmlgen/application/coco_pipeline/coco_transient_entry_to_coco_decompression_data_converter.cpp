#include "asmlgen/application/coco_pipeline/coco_transient_entry_to_coco_decompression_data_converter.h"

namespace pipeline
{
CocoDecompressionData* CocoTransientEntryToCocoDecompressionDataConverter::Call(
  TransientCocoEntry* transient_coco_entry)
{
  const std::string& url = transient_coco_entry->GetURL();
  std::string filename = url.substr(url.find_last_of('/') + 1, url.size());
  size_t offset = filename.find_last_of('.');
  filename.insert(offset, "_mask");

  coco_decompression_data_ = { transient_coco_entry->AnnotationsData(),
    transient_coco_entry->GetMaskIdValuesName(),
    transient_coco_entry->GetImageWidth(),
    transient_coco_entry->GetImageHeight(),
    filename,
    transient_coco_entry->GetWritePath() };

  return &coco_decompression_data_;
}

} // namespace pipeline
