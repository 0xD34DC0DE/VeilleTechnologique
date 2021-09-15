#include "asmlgen/application/tasking/task_pool.h"

namespace tasking
{

TaskPool::TaskPool(
  std::vector<std::string> urls, std::shared_ptr<Orchestrator> orchestrator_ptr, uint32_t max_concurrent_tasks)
  : urls_(urls), task_waiting_queue_(max_concurrent_tasks, std::move(orchestrator_ptr)), dispatched_task_count_(0),
    dispatched_task_max_count_(max_concurrent_tasks)
{
  tasks_.reserve(dispatched_task_count_);

  for (uint32_t i = 0; i < max_concurrent_tasks; ++i)
  {
    auto&& url = std::move(urls_.top());

    tasks_.emplace_back(Task { std::move(url) });

    urls_.pop();
  }
}

void TaskPool::Start()
{
  std::scoped_lock<std::mutex> lock(task_pool_mutex);
  if (!is_running_)
  {
    is_running_ = true;

    uint32_t dispatch_count = dispatched_task_max_count_ - dispatched_task_count_;
    dispatched_task_count_ = dispatch_count;

    for (uint32_t i = 0; i < dispatch_count; ++i)
    {
      auto&& task = std::move(tasks_.back());
      task_waiting_queue_.AddTask(std::move(task));
      tasks_.pop_back();
    }
  }
}

void TaskPool::Stop()
{
  std::scoped_lock<std::mutex> lock(task_pool_mutex);
  is_running_ = false;
}

bool TaskPool::IsDone()
{
  std::scoped_lock<std::mutex> lock(task_pool_mutex);
  return task_waiting_queue_.IsEmpty() && tasks_.empty() && dispatched_task_count_ == 0;
}

void TaskPool::RejoinPool(Task&& task)
{
  std::scoped_lock<std::mutex> lock(task_pool_mutex);

  if (is_running_)
  {
    auto&& url = std::move(urls_.top());
    task.SetUrl(url);
    urls_.pop();
    task_waiting_queue_.AddTask(std::move(task));
  }
  else
  {
    auto&& url = std::move(urls_.top());
    task.SetUrl(url);
    tasks_.emplace_back(std::move(task));
    urls_.pop();
  }
}

} // namespace tasking