#ifndef ASMLGEN_TASK_POOL_H
#define ASMLGEN_TASK_POOL_H

#include "asmlgen/application/tasking/task.h"
#include "asmlgen/application/tasking/task_waiting_queue.h"

#include <vector>

namespace tasking
{

// TODO add counter for dispatched tasks and completed tasks
class TaskPool
{
public:
  // Use zero to use max number of threads
  explicit TaskPool(
    std::vector<std::string> urls, std::shared_ptr<Orchestrator> orchestrator_ptr, uint32_t max_concurrent_tasks = 0);

  ///
  /// Start dispatching tasks
  ///
  void Start();

  ///
  /// Stop dispatching tasks
  ///
  void Stop();

  ///
  /// Returns true if all tasks have been dispatched
  ///
  bool IsDone();

private:
  TaskWaitingQueue task_waiting_queue_;
  uint32_t dispatched_task_count_;

  std::vector<std::string> urls_;
  std::vector<Task> tasks_;
};

} // namespace tasking

#endif // ASMLGEN_TASK_POOL_H
