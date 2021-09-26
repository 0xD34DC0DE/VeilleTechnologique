#include "asmlgen/application/tasking/refactor/task_manager.h"

#define SFML_STATIC
#include <SFML/Network.hpp>

#include <iostream>

using namespace tasking;

struct DownloadTask
{
  std::string output_filepath;
  std::string image_url;
};

struct DownloadInfo
{
  std::string host;
  std::string resource;
  std::size_t download_size;
  std::string output_filepath;
};

class FetchDownloadSize : public MakeChainObj<FetchDownloadSize, DownloadInfo, DownloadTask>
{
public:
  DownloadInfo Call(DownloadTask download_task)
  {

    auto host_and_resource = SplitHostAndResource(download_task.image_url);
    DownloadInfo download_info = {
      host_and_resource.first, host_and_resource.second, 0, download_task.output_filepath
    };
    std::cout << "starting fetch" << std::endl;

    sf::Http http(download_info.host);
    sf::Http::Request request(download_info.resource, sf::Http::Request::Head);
    sf::Http::Response response = http.sendRequest(request, sf::seconds(2));

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

struct DownloadedImage
{
  std::string output_filepath;
  std::vector<uint8_t> image_bytes;
};

class DownloadImage : public MakeChainObj<DownloadImage, DownloadedImage, DownloadInfo>
{
public:
  DownloadedImage Call(DownloadInfo download_info)
  {
    if (download_info.download_size > 0)
    {
      std::cout << "starting download" << std::endl;

      DownloadedImage downloaded_image { download_info.output_filepath,
        std::move(std::vector<uint8_t>(download_info.download_size)) };

      sf::Http http(download_info.host);
      sf::Http::Request request(download_info.resource, sf::Http::Request::Get);
      sf::Http::Response response = http.sendRequest(request, sf::seconds(2));

      const std::string& body_str = response.getBody();

      std::copy(body_str.begin(), body_str.end(), downloaded_image.image_bytes.begin());

      return downloaded_image;
    }
    else
    {
      std::cout << "Failed to download: " << download_info.host << download_info.resource << std::endl;
      return { "", {} };
    }
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

  TaskManager task_manager = TaskManager<FetchDownloadSize>(2);
  task_manager.SetInputs(urls);
  task_manager.Start();

  while (!task_manager.AllTasksDone()) {}
}

// 9:17 to 12:00