#include "asmlgen/application/generic_pipeline_utils/download_file.h"

#define SFML_STATIC
#include <SFML/Network.hpp>

namespace pipeline
{

DownloadData* DownloadFile::Call(DownloadData* download_data)
{
  if (download_data->HasFailed()) return download_data;

  std::vector<uint8_t> data;
  if (download_data->GetDownloadSize() > 0)
  {
    data.reserve(download_data->GetDownloadSize());
    data.resize(download_data->GetDownloadSize());
  }

  for (int i = 0; i < 3; ++i)
  {
    sf::Http http(download_data->GetHost());
    sf::Http::Request request(download_data->GetResource(), sf::Http::Request::Get);
    sf::Http::Response response = http.sendRequest(request, sf::seconds(15));

    if (response.getStatus() == sf::Http::Response::Ok)
    {
      if (download_data->GetDownloadSize() == 0) { data.resize(response.getBody().size()); }
      const std::string& body_str = response.getBody();

      std::copy(body_str.begin(), body_str.end(), data.begin());

      download_data->SetData(std::move(data));
      break;
    }
    else
    {
      download_data->MarkFailed();
    }
  }
  return download_data;
}

} // namespace pipeline