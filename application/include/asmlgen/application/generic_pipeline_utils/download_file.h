#ifndef ASMLGEN_DOWNLOAD_FILE_H
#define ASMLGEN_DOWNLOAD_FILE_H

#include "asmlgen/application/generic_pipeline_utils/download_data.h"
#include "asmlgen/application/tasking/call_chain.h"

#define SFML_STATIC
#include <SFML/Network.hpp>

namespace pipeline
{

class DownloadFile : public tasking::MakeChainObj<DownloadFile, DownloadData*, DownloadData*>
{
public:
  DownloadData* Call(DownloadData* download_data);
};
} // namespace pipeline

#endif // ASMLGEN_DOWNLOAD_FILE_H
