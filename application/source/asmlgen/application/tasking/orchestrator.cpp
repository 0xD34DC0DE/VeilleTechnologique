#include "asmlgen/application/tasking/orchestrator.h"

namespace tasking
{

Orchestrator::Orchestrator(const size_t& max_resource, TaskWaitingQueue& waiting_queue)
  : available_resources_(max_resource), waiting_queue_(waiting_queue), has_waiting_queue_signaled_(false),
    has_task_ended_(false), is_task_parked_(false)
{}

void Orchestrator::AcquireTask()
{
  waiting_queue_.GetNextTask();
}

void Orchestrator::SignalNewTask() {}

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
  condition_variable_.wait(lock, stop_token_, [this] { return has_task_ended_ || has_waiting_queue_signaled_; });
}

void Orchestrator::Update()
{
  std::scoped_lock<std::mutex> lock(orchestrator_mutex_);

  // TODO simplify/refactor state machine

  if (has_task_ended_ || has_waiting_queue_signaled_)
  {
    if (is_task_parked_)
    {
      if (parked_task_.GetRequiredResources() <= available_resources_)
      {
        parked_task_.SetCompletionCallback(
          [this](std::size_t released_resources) { SignalTaskDone(released_resources); });

        available_resources_ -= parked_task_.GetRequiredResources();

        parked_task_.RunTask();

        running_tasks.emplace_back(std::move(parked_task_));

        is_task_parked_ = false;
      }
      else
      {
        return; // Back to waiting
      }
    }

    while (!is_task_parked_ && !waiting_queue_.IsEmpty())
    {
      parked_task_ = std::move(waiting_queue_.GetNextTask());
      is_task_parked_ = true;

      if (parked_task_.GetRequiredResources() <= available_resources_)
      {
        parked_task_.SetCompletionCallback(
          [this](std::size_t released_resources) { SignalTaskDone(released_resources); });

        available_resources_ -= parked_task_.GetRequiredResources();

        parked_task_.RunTask();

        running_tasks.emplace_back(std::move(parked_task_));

        is_task_parked_ = false;
      }
      else
      {
        return; // Back to waiting
      }
    }
  }
  // Back to waiting
}

void Orchestrator::SignalTaskDone(std::size_t resources_released)
{
  std::scoped_lock<std::mutex> lock(orchestrator_mutex_);

  available_resources_ += resources_released;
  has_task_ended_ = true;

  condition_variable_.notify_one();
}

} // namespace tasking
