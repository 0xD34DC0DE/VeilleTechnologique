#ifndef ASMLGEN_FETCH_DOWNLOAD_SIZE_H
#define ASMLGEN_FETCH_DOWNLOAD_SIZE_H

#include "asmlgen/application/generic_pipeline_utils/download_data.h"
#include "asmlgen/application/tasking/call_chain.h"

namespace pipeline
{

class FetchDownloadSize : public tasking::MakeChainObj<FetchDownloadSize, DownloadData*, DownloadData*>
{
public:
  DownloadData* Call(DownloadData* download_data);
};

} // namespace pipeline
#endif
