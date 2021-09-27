#include "asmlgen/application/generic_pipeline_utils/fetch_download_size.h"

#define SFML_STATIC
#include <SFML/Network.hpp>

namespace pipeline
{

DownloadData* FetchDownloadSize::Call(DownloadData* download_data)
{
  sf::Http http(download_data->GetHost());
  sf::Http::Request request(download_data->GetResource(), sf::Http::Request::Head);
  sf::Http::Response response = http.sendRequest(request, sf::seconds(2));

  if (response.getStatus() == sf::Http::Response::Ok)
  {
    download_data->SetDownloadSize(std::stoul(response.getField("content-length")));
    return download_data;
  }
  else
  {
    download_data->MarkFailed();
  }
}

} // namespace pipeline