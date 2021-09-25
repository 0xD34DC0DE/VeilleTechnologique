#include "asmlgen/application/tasking/refactor/reusable_thread.h"

#define SFML_STATIC
#include <iostream>
#include <SFML/Network.hpp>

#include <chrono>

using namespace tasking;

struct DownloadInfo
{
  std::string host;
  std::string resource;
  std::size_t download_size;
};

class FetchDownloadSize : public MakeChainObj<FetchDownloadSize, DownloadInfo, std::string>
{
public:
  FetchDownloadSize() {}

  DownloadInfo Call(std::string url)
  {

    auto host_and_resource = SplitHostAndResource(url);
    DownloadInfo download_info = { host_and_resource.first, host_and_resource.second, 0 };
    std::cout << "starting fetch" << std::endl;

    sf::Http http(download_info.host);
    sf::Http::Request request(download_info.resource, sf::Http::Request::Head);
    sf::Http::Response response = http.sendRequest(request, sf::seconds(1));

    download_info.download_size = std::stoul(response.getField("content-length"));
    return download_info;
  }

private:
  std::pair<std::string, std::string> SplitHostAndResource(const std::string& url)
  {
    std::size_t protocol_end = url.find_first_of(':') + 3; // start of host is after "://" -> +3
    std::size_t host_end = url.find_first_of('/', protocol_end);

    return { url.substr(0, host_end), url.substr(host_end) };
  }
};

int main(int, char**)
{

  std::vector<std::string> urls { "http://images.cocodataset.org/val2017/000000114907.jpg",
    "http://images.cocodataset.org/val2017/000000115245.jpg",
    "http://images.cocodataset.org/val2017/000000437898.jpg",
    "http://images.cocodataset.org/val2017/000000470952.jpg",
    "http://images.cocodataset.org/val2017/000000481413.jpg",
    "http://images.cocodataset.org/val2017/000000451150.jpg",
    "http://images.cocodataset.org/val2017/000000149222.jpg",
    "http://images.cocodataset.org/val2017/000000507081.jpg" };

  std::atomic_uint32_t done_count = 0;

  using DownloadingReusableThreads = ReusableThread<decltype(MakeChained(FetchDownloadSize()))>;
  std::vector<std::unique_ptr<DownloadingReusableThreads>> a;

  using namespace std::chrono_literals;

  for (const std::string& url : urls)
  {
    a.emplace_back(std::make_unique<DownloadingReusableThreads>(MakeChained(FetchDownloadSize()),
                     [&done_count](DownloadInfo download_info)
                     {
                       std::cout << "done fetching download size: " << download_info.download_size << " bytes"
                                 << std::endl;
                       done_count++;
                     }))
      ->Start(url);
  }

  while (done_count != urls.size()) {}
  for (auto& rthread : a)
  {
    rthread->Terminate();
  }
}