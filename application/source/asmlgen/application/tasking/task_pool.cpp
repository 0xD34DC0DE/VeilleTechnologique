#include "asmlgen/application/tasking/task_pool.h"
#include <iostream>

namespace tasking
{

TaskPool::TaskPool(
  std::vector<std::string> urls, std::shared_ptr<Orchestrator> orchestrator_ptr, uint32_t max_concurrent_tasks)
  : urls_(urls), task_waiting_queue_(max_concurrent_tasks, std::move(orchestrator_ptr)), dispatched_task_count_(0),
    dispatched_task_max_count_(max_concurrent_tasks), is_running_(false)
{
  // tasks_.reserve(dispatched_task_count_);

  for (uint32_t i = 0; i < max_concurrent_tasks; ++i)
  {
    std::cout << "Task Pool: Create New task" << std::endl;
    auto&& url = std::move(urls_.top());

    tasks_.emplace_back(Task { std::move(url) });

    urls_.pop();
  }
}

void TaskPool::Start()
{
  std::cout << "LOCKING" << std::endl << std::flush;
  std::scoped_lock<std::mutex> lock(task_pool_mutex);

  std::cout << "Task Pool: Start" << std::endl << std::flush;

  if (!is_running_)
  {
    is_running_ = true;

    uint32_t dispatch_count = dispatched_task_max_count_ - dispatched_task_count_;
    dispatched_task_count_ = dispatch_count;

    std::cout << "Task Pool: Starting dispatch: " << dispatch_count << std::endl;

    for (uint32_t i = 0; i < dispatch_count; ++i)
    {
      auto&& task = std::move(tasks_.back());
      std::cout << "Task Pool: Give queue new task" << std::endl;
      task.RunPrologue();
      task_waiting_queue_.AddTask(std::move(task));
      tasks_.pop_back();
    }
  }
}

void TaskPool::Stop()
{
  std::scoped_lock<std::mutex> lock(task_pool_mutex);
  std::cout << "Task Pool: Terminate" << std::endl;
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

  std::cout << "Task Pool: task rejoin: " << task.GetId() << std::endl;

  if (is_running_)
  {
    auto&& url = std::move(urls_.top());
    task.SetUrl(url);
    urls_.pop();
    task.RunPrologue();
    task_waiting_queue_.AddTask(std::move(task));
  }
  else
  {
    auto&& url = std::move(urls_.top());
    task.SetUrl(url);
    tasks_.emplace_back(std::move(task));
    task.RunPrologue();
    urls_.pop();
  }
}

TaskWaitingQueue* TaskPool::GetTaskWaitingQueuePtr()
{
  return &task_waiting_queue_;
}

} // namespace tasking