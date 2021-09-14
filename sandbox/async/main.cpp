

#define SFML_STATIC
#include <SFML/Network.hpp>

#include <future>
#include <mutex>
#include <vector>

struct Downloader
{
  // To prevent concurrent access to the images bytes vector
  std::mutex mutex_;

  void DownloadImage(const std::string& url)
  {

    sf::Http http(url);
    sf::Http::Request request("", sf::Http::Request::Get);
    sf::Http::Response response = http.sendRequest(request, sf::seconds(2));

    std::scoped_lock<std::mutex> mutex(mutex_);
    if (response.getStatus() == sf::Http::Response::Ok)
    {
      const auto& response_str = response.getBody();
      images_bytes.emplace_back(std::vector<uint8_t>(response_str.begin(), response_str.end()));
    }
    else
    {
      images_bytes.emplace_back();
    }
  }

  std::vector<std::vector<uint8_t>> images_bytes;
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

  Downloader downloader;

  std::vector<std::future<void>> futures;
  for (const auto& url : urls)
    futures.emplace_back(std::async(&Downloader::DownloadImage, &downloader, url));
}