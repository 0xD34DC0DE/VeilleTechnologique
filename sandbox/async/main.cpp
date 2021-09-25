
#include "asmlgen/application/tasking/refactor/call_chain.h"

#define SFML_STATIC
#include <SFML/Network.hpp>

#include <iostream>
#include <memory>
#include <mutex>
#include <vector>

using namespace tasking;

struct Download
{
  std::string url;
  uint64_t size;
};

struct GetDownloadSize : MakeChainObj<GetDownloadSize, Download, std::string>
{
  Download Call(std::string url)
  {
    Download d = { url, 0 };
    std::cout << url << std::endl;
    sf::Http http("http://images.cocodataset.org"); // Temporary
    sf::Http::Request request(url, sf::Http::Request::Head);
    sf::Http::Response response = http.sendRequest(request, sf::seconds(5));
    d.size = std::stoul(response.getField("content-length"));
    return d;
  }
};

struct DownloadFile : MakeChainObj<GetDownloadSize, uint64_t, Download>
{
  uint64_t Call(Download download_info)
  {
    std::vector<uint8_t> bytes;
    bytes.reserve(download_info.size);

    sf::Http http("http://images.cocodataset.org"); // Temporary
    sf::Http::Request request(download_info.url, sf::Http::Request::Get);
    sf::Http::Response response = http.sendRequest(request, sf::seconds(5));
    for (int i = 0; i < response.getBody().size(); ++i)
    {
      bytes.emplace_back(response.getBody()[i]);
    }
    return bytes.size();
  }
};

int main(int, char**)
{

  std::vector<std::string> urls { "/val2017/000000114907.jpg",
    "/val2017/000000115245.jpg",
    "/val2017/000000437898.jpg",
    "/val2017/000000470952.jpg",
    "/val2017/000000481413.jpg",
    "/val2017/000000451150.jpg",
    "/val2017/000000149222.jpg",
    "/val2017/000000507081.jpg" };

  std::vector<std::jthread> threads;

  for (int i = 0; i < 7; ++i)
  {
    threads.emplace_back(
      [&urls, &i]
      {
        auto f1 = GetDownloadSize();
        auto f2 = DownloadFile();
        auto a = MakeChained(f1, f2);
        std::cout << "Downloaded: " << a.Start(urls[i]) << " bytes";
      });
  }

  for (auto& t : threads)
  {
    t.join();
  }

  //  std::shared_ptr<Orchestrator> orchestrator_ptr = std::make_shared<Orchestrator>(1024 * 2048);
  //  while (!orchestrator_ptr->IsReady()) {};
  //
  //  auto* task_pool = new TaskPool(urls, orchestrator_ptr, 3);
  //  std::cout << "Task Pool: created" << std::endl;
  //
  //  orchestrator_ptr->SetTaskWaitingQueue(task_pool->GetTaskWaitingQueuePtr());
  //  orchestrator_ptr->SetTaskPool(task_pool);
  //
  //  task_pool->Start();
  //  std::cout << "Task Pool: started" << std::endl;
  //
  //  using namespace std::chrono_literals;
  //  while (!task_pool->IsDone())
  //  {
  //    std::this_thread::sleep_for(500ms);
  //    std::cout << "Main thread: sleep" << std::endl;
  //  }

  //  task_pool->Stop();
}