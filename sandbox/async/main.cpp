
#include "asmlgen/application/tasking/orchestrator.h"
#include "asmlgen/application/tasking/task_pool.h"

#define SFML_STATIC
#include <SFML/Network.hpp>

#include <iostream>
#include <memory>
#include <mutex>
#include <vector>

int main(int, char**)
{

  using namespace tasking;

  std::vector<std::string> urls { "/val2017/000000114907.jpg",
    "/val2017/000000115245.jpg",
    "/val2017/000000437898.jpg",
    "/val2017/000000470952.jpg",
    "/val2017/000000481413.jpg",
    "/val2017/000000451150.jpg",
    "/val2017/000000149222.jpg",
    "/val2017/000000507081.jpg" };

  std::shared_ptr<Orchestrator> orchestrator_ptr = std::make_shared<Orchestrator>(1024 * 2048);
  while (!orchestrator_ptr->IsReady()) {};

  auto* task_pool = new TaskPool(urls, orchestrator_ptr, 3);
  std::cout << "Task Pool: created" << std::endl;

  orchestrator_ptr->SetTaskWaitingQueue(task_pool->GetTaskWaitingQueuePtr());
  orchestrator_ptr->SetTaskPool(task_pool);

  task_pool->Start();
  std::cout << "Task Pool: started" << std::endl;

  using namespace std::chrono_literals;
  while (!task_pool->IsDone())
  {
    std::this_thread::sleep_for(500ms);
    std::cout << "Main thread: sleep" << std::endl;
  }

  task_pool->Stop();
}