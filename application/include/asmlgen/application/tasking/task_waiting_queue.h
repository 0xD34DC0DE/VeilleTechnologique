#ifndef ASMLGEN_TASK_WAITING_QUEUE_H
#define ASMLGEN_TASK_WAITING_QUEUE_H

#include "asmlgen/application/tasking/task.h"

#include <cstdint>
#include <memory>
#include <mutex>
#include <queue>

namespace tasking
{

class Orchestrator; // Forward declare to avoid recursive inclusion

class TaskWaitingQueue
{
public:
  explicit TaskWaitingQueue(uint32_t max_task_count, std::shared_ptr<Orchestrator> orchestrator_ptr);

  void AddTask(Task&& task);

  /// Return the next waiting task in the queue
  ///
  /// @warning Make sure that the queue is not empty by using IsEmpty()
  ///
  /// @return Next task rvalue reference in the queue
  Task&& GetNextTask();

  bool IsEmpty();

  uint64_t WaitingTaskCount();

private:
  uint32_t waiting_task_count_;
  std::queue<Task> task_queue_;
  std::mutex task_queue_mutex;

  std::shared_ptr<Orchestrator> orchestrator_ptr_;
};

} // namespace tasking

#endif // ASMLGEN_TASK_WAITING_QUEUE_H
