#include "asmlgen/application/tasking/task_pool.h"

namespace tasking
{

TaskPool::TaskPool(
  std::vector<std::string> urls, std::shared_ptr<Orchestrator> orchestrator_ptr, uint32_t max_concurrent_tasks)
  : urls_(urls), task_waiting_queue_(max_concurrent_tasks, std::move(orchestrator_ptr)),
    dispatched_task_count_(max_concurrent_tasks)
{
  tasks_.reserve(dispatched_task_count_);
}

void TaskPool::Start() {}

void TaskPool::Stop() {}

bool TaskPool::IsDone()
{
  return false;
}

} // namespace tasking