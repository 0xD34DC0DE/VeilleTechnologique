#include "asmlgen/application/coco_pipeline/transient_coco_entry.h"

namespace dataset
{

void TransientCocoEntry::ProcessData() {}

std::size_t TransientCocoEntry::AcquireData()
{
  return 0;
}

std::string TransientCocoEntry::GetWritePath() const noexcept
{
  return write_path_;
}
} // namespace dataset