#ifndef ASMLGEN_ORCHESTRATOR_H
#define ASMLGEN_ORCHESTRATOR_H

#include "asmlgen/application/tasking/task_waiting_queue.h"

#include <condition_variable>
#include <cstddef>
#include <list>
#include <mutex>
#include <thread>

namespace tasking
{

class TaskWaitingQueue; // Forward declare to avoid recursive inclusion

class Orchestrator
{
public:
  Orchestrator(const std::size_t& max_resource, TaskWaitingQueue& waiting_queue);

  /// Tell the orchestrator that new tasks are ready to be pulled from the waiting queue
  void SignalNewTask();

private:
  std::atomic_size_t available_resources_;
  TaskWaitingQueue& waiting_queue_;

  // Next task to be started
  Task parked_task_;
  bool is_task_parked_;

  std::list<Task> running_tasks;

  bool has_waiting_queue_signaled_;
  bool has_task_ended_;

  /// Thread used to perform the orchestration of task's threads separately from the main thread
  std::jthread orchestrator_thread_;

  std::mutex orchestrator_mutex_;
  std::condition_variable_any condition_variable_;
  std::stop_token stop_token_;

  void AcquireTask();

  ///
  /// Internal loop to switch between updating and waiting until the stop token is signaled to stop
  ///
  /// @note This should only be called from the orchestrator's thread
  ///
  void Loop();

  ///
  /// Wait for a task to finish or for the waiting queue to signal new work
  ///
  /// @note This should only be called from the orchestrator's thread
  ///
  void Wait();

  ///
  /// Update the internal state and perform actions accordingly
  ///
  /// @note This should only be called from the orchestrator's thread
  void Update();

  ///
  /// Used by running task to signal their completion and the resource they release
  ///
  void SignalTaskDone(std::size_t resources_released);
};

} // namespace tasking

#endif // ASMLGEN_ORCHESTRATOR_H
