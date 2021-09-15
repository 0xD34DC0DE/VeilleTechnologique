#include "asmlgen/application/tasking/task_waiting_queue.h"

#include <iostream>
#include <utility>

#include "asmlgen/application/config/assertion.h"
#include "asmlgen/application/tasking/orchestrator.h"

namespace tasking
{

TaskWaitingQueue::TaskWaitingQueue(uint32_t max_task_count, std::shared_ptr<Orchestrator> orchestrator_ptr)
  : waiting_task_count_(max_task_count), orchestrator_ptr_(std::move(orchestrator_ptr))
{}

void TaskWaitingQueue::AddTask(Task&& task)
{
  std::scoped_lock<std::mutex> task_queue_lock(task_queue_mutex);

  std::cout << "Task Queue: Add task" << std::endl;

  bool was_empty = task_queue_.empty();
  std::cout << "Task Queue: was empty: " << task_queue_.empty() << std::endl;

  task_queue_.emplace(std::move(task));

  if (was_empty)
  {
    std::cout << "Task Queue: Signal new task" << std::endl;
    orchestrator_ptr_->SignalNewTask();
  }
  else
  {
    std::cout << "Task Queue: Not empty: " << task_queue_.size() << std::endl << std::flush;
  }
}

Task&& TaskWaitingQueue::GetNextTask()
{
  std::scoped_lock<std::mutex> task_queue_lock(task_queue_mutex);
  ASSERT(!task_queue_.empty(), "GetNextTask was called but the queue is empty");

  std::cout << "Task Queue: Give new task" << std::endl;

  auto&& task = std::move(task_queue_.front());
  task_queue_.pop();
  return std::move(task);
}

bool TaskWaitingQueue::IsEmpty()
{
  std::scoped_lock<std::mutex> task_queue_lock(task_queue_mutex);
  return task_queue_.empty();
}

uint64_t TaskWaitingQueue::WaitingTaskCount()
{
  std::scoped_lock<std::mutex> task_queue_lock(task_queue_mutex);
  return task_queue_.size();
}

} // namespace tasking