#include "asmlgen/application/tasking/task_manager.h"

#include "asmlgen/application/config/assertion.h"

#define SFML_STATIC
#include <SFML/Network.hpp>

#include <filesystem>
#include <fstream>
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

    DownloadInfo download_info = { std::move(host_and_resource.first),
      std::move(host_and_resource.second),
      0,
      std::move(download_task.output_filepath) };

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

      DownloadedImage downloaded_image { std::move(download_info.output_filepath),
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

class WriteImage : public MakeChainObj<WriteImage, bool, DownloadedImage>
{
public:
  bool Call(DownloadedImage downloaded_image)
  {
    if (!downloaded_image.output_filepath.empty())
    {
      std::cout << "starting writing" << std::endl;

      if (!std::filesystem::exists(downloaded_image.output_filepath))
      {
        std::ofstream output_file_stream(downloaded_image.output_filepath, std::ios::out | std::ios::binary);
        if (!output_file_stream)
        {
          ASSERT(false, std::string("Could not write file to path: ").append(downloaded_image.output_filepath));
        }

        output_file_stream.write(
          reinterpret_cast<const char*>(downloaded_image.image_bytes.data()), downloaded_image.image_bytes.size());
        return true;
      }
    }
    else
    {
      std::cout << "Skipping write, empty output_filepath" << std::endl;
    }
    return false;
  }
};

int main(int, char**)
{
  std::string output_directory_path = "C:/Users/0xD34DC0DE/Pictures/ASMLGen/output_test";
  std::vector<std::string> urls { "http://images.cocodataset.org/val2017/000000114907.jpg",
    "http://images.cocodataset.org/val2017/000000115245.jpg",
    "http://images.cocodataset.org/val2017/000000437898.jpg",
    "http://images.cocodataset.org/val2017/000000470952.jpg",
    "http://images.cocodataset.org/val2017/000000481413.jpg",
    "http://images.cocodataset.org/val2017/000000451150.jpg",
    "http://images.cocodataset.org/val2017/000000149222.jpg",
    "http://images.cocodataset.org/val2017/000000507081.jpg" };

  std::vector<DownloadTask> download_tasks;

  for (const auto& url : urls)
  {
    std::string output_path = output_directory_path + url.substr(url.find_last_of('/'), url.size());
    download_tasks.push_back({ output_path, url });
  }

  TaskManager task_manager = TaskManager<FetchDownloadSize, DownloadImage, WriteImage>(2);
  task_manager.SetInputs(download_tasks);
  task_manager.Start();

  while (!task_manager.AllTasksDone()) {}
}

// 9:17 to 12:00