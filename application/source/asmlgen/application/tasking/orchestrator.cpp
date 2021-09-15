#include "asmlgen/application/tasking/orchestrator.h"
#include <iostream>

#include "asmlgen/application/tasking/task_pool.h"

namespace tasking
{

Orchestrator::Orchestrator(const size_t& max_resource)
  : available_resources_(max_resource), has_waiting_queue_signaled_(false), has_task_ended_(false),
    is_task_parked_(false)
{
  std::cout << "Orchestrator: Constructed" << std::endl;
  orchestrator_thread_ = std::jthread(&Orchestrator::Loop, this);
}

void Orchestrator::AcquireTask()
{
  std::cout << "Orchestrator: Acquire task" << std::endl;
  waiting_queue_->GetNextTask();
}

void Orchestrator::SignalNewTask()
{
  std::unique_lock<std::mutex> lock(orchestrator_mutex_);

  std::cout << "Orchestrator: New task signaled" << std::endl;
  has_waiting_queue_signaled_ = true;
  condition_variable_.notify_one();
}

void Orchestrator::Loop()
{
  do
  {
    Update();
    Wait();
  }
  while (!stop_token_.stop_requested());
}

void Orchestrator::Wait()
{
  std::unique_lock<std::mutex> lock(orchestrator_mutex_);
  std::cout << "Orchestrator: Waiting" << std::endl;
  is_ready_ = true;
  condition_variable_.wait(lock,
    stop_token_,
    [this]
    {
      std::cout << "Orchestrator: Predicate called -> ended sig:" << has_task_ended_ << " new sig "
                << has_waiting_queue_signaled_ << std::endl;
      return has_task_ended_ || has_waiting_queue_signaled_;
    });
  std::cout << "Orchestrator: Woken" << std::endl;
}

void Orchestrator::Update()
{
  std::scoped_lock<std::mutex> lock(orchestrator_mutex_);

  // TODO simplify/refactor state machine

  if (has_task_ended_ || has_waiting_queue_signaled_)
  {
    std::cout << "Orchestrator: lunching tasks" << std::endl;
    if (is_task_parked_)
    {
      std::cout << "Orchestrator: parked tasks, checking resource" << std::endl;
      if (parked_task_.GetRequiredResources() <= available_resources_)
      {
        std::cout << "Orchestrator: enough resource" << std::endl;
        parked_task_.SetCompletionCallback(
          [this](std::size_t released_resources, uint64_t task_id) { SignalTaskDone(released_resources, task_id); });

        available_resources_ -= parked_task_.GetRequiredResources();

        std::cout << "Orchestrator: Running new task" << std::endl;

        running_tasks.emplace_back(std::move(parked_task_));

        running_tasks.back().RunTask();

        is_task_parked_ = false;
      }
      else
      {
        return; // Back to waiting
      }
    }

    std::cout << "Orchestrator: checking if queue empty: " << waiting_queue_->IsEmpty() << std::endl;
    while (!is_task_parked_ && !waiting_queue_->IsEmpty())
    {
      std::cout << "Orchestrator: Queue is not empty and no parked tasks, starting new task" << std::endl;
      parked_task_ = std::move(waiting_queue_->GetNextTask());
      is_task_parked_ = true;

      if (parked_task_.GetRequiredResources() <= available_resources_)
      {
        parked_task_.SetCompletionCallback(
          [this](std::size_t released_resources, uint64_t task_id) { SignalTaskDone(released_resources, task_id); });

        available_resources_ -= parked_task_.GetRequiredResources();

        std::cout << "Orchestrator: Running new task" << std::endl;

        running_tasks.emplace_back(std::move(parked_task_));

        running_tasks.back().RunTask();

        is_task_parked_ = false;
      }
      else
      {
        return; // Back to waiting
      }
    }
  }

  has_task_ended_ = false;
  has_waiting_queue_signaled_ = false;
  // Back to waiting
}

void Orchestrator::SignalTaskDone(std::size_t resources_released, uint64_t task_id)
{
  std::unique_lock<std::mutex> lock(orchestrator_mutex_);

  std::cout << "Orchestrator: Task done signaled" << std::endl;

  available_resources_ += resources_released;
  has_task_ended_ = true;

  auto task_it = std::find_if(
    running_tasks.begin(), running_tasks.end(), [task_id](const Task& task) { return task.GetId() == task_id; });

  task_pool_ptr_->RejoinPool(std::move(*task_it));

  running_tasks.erase(task_it);

  condition_variable_.notify_one();
}

void Orchestrator::SetTaskWaitingQueue(TaskWaitingQueue* waiting_queue)
{
  waiting_queue_ = waiting_queue;
}

void Orchestrator::SetTaskPool(TaskPool* task_pool_ptr)
{
  task_pool_ptr_ = task_pool_ptr;
}
bool Orchestrator::IsReady()
{
  return is_ready_;
}

} // namespace tasking
