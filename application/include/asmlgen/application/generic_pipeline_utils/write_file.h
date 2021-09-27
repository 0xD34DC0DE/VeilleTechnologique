#ifndef ASMLGEN_WRITE_FILE_H
#define ASMLGEN_WRITE_FILE_H

#include "asmlgen/application/generic_pipeline_utils/file_write_data.h"
#include "asmlgen/application/tasking/call_chain.h"

namespace pipeline
{

class WriteFile : public tasking::MakeChainObj<WriteFile, std::size_t, FileWriteData*>
{
public:
  [[nodiscard]] std::size_t Call(FileWriteData* file_write_data) const;
};

} // namespace pipeline
#endif
