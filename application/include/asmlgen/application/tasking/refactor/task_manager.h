#ifndef ASMLGEN_TASK_MANAGER_H
#define ASMLGEN_TASK_MANAGER_H

#include "asmlgen/application/tasking/refactor/reusable_thread.h"

#include <iostream>
#include <stack>
#include <type_traits>

namespace tasking
{

template<Chainable... Chainables>
class TaskManager
{
  using Chain_T = decltype(MakeChained(std::declval<Chainables>()...));
  using TaskReusableThread_T = ReusableThread<Chain_T>;
  using TaskReusableThreadPtr_T = std::unique_ptr<TaskReusableThread_T>;
  using Output_T = typename Chain_T::OutputType;
  using Input_T = typename Chain_T::InputType;

public:
  // Use max_threads 0 to use the maximum number of threads the implementation can support minus 2
  TaskManager(uint32_t max_threads = 0)
    : all_task_done_(false), should_stop_(false), running_reusable_thread_count_(0),
      thread_(std::jthread(&TaskManager<Chainables...>::Loop, this))
  {
    if (!max_threads)
    {
      max_threads = std::jthread::hardware_concurrency();
      if (max_threads == 0)
      {
        max_threads = 1; // Fallback value
      }
    }

    for (uint32_t i = 0; i < max_threads; ++i)
    {
      reusable_threads_.emplace_back(std::make_unique<TaskReusableThread_T>(MakeChained(Chainables {}...),
        [this](std::thread::id thread_id, Output_T result) { CollectResult(thread_id, result); }));
    }
  }

  ~TaskManager()
  {
    for (TaskReusableThreadPtr_T& thread : reusable_threads_)
    {
      thread->Terminate();
    }
  }

  void SetInputs(const std::vector<Input_T>& inputs)
  {
    inputs_ = inputs;
  }

  void Start()
  {
    std::scoped_lock lock(mutex_);

    input_index = 0;

    std::size_t initial_calls = std::min(reusable_threads_.size(), inputs_.size());
    for (std::size_t i = 0; i < initial_calls; ++i)
    {
      reusable_threads_[i]->Start(inputs_[input_index++]);
      ++running_reusable_thread_count_;
    }
  }

  [[nodiscard]] bool AllTasksDone()
  {
    std::scoped_lock lock(mutex_);
    return inputs_.size() == outputs_.size();
  }

private:
  std::vector<TaskReusableThreadPtr_T> reusable_threads_;
  std::stack<std::thread::id> waiting_reusable_treads_ids_;
  uint32_t running_reusable_thread_count_;

  std::jthread thread_;
  std::mutex mutex_;
  std::condition_variable condition_variable_;

  bool all_task_done_;
  bool should_stop_;

  std::vector<Input_T> inputs_;
  std::size_t input_index;

  std::vector<Output_T> outputs_;

  void CollectResult(std::thread::id thread_id, Output_T result)
  {
    std::unique_lock lock(mutex_);

    waiting_reusable_treads_ids_.emplace(thread_id);
    std::cout << "fetch size: " << std::dec << result.download_size << ", from: 0x" << std::hex << thread_id
              << std::endl;

    outputs_.emplace_back(result);

    condition_variable_.notify_one();
  }

  void Wait()
  {
    std::unique_lock lock(mutex_);
    condition_variable_.wait(lock, [this] { return (waiting_reusable_treads_ids_.size() > 0) | should_stop_; });
  }

  void StartNewTasks()
  {
    std::scoped_lock lock(mutex_);

    if (input_index == inputs_.size())
    {
      if (--running_reusable_thread_count_ == 0) { all_task_done_ = true; }
    }
    else
    {
      while (waiting_reusable_treads_ids_.size() > 0)
      {
        if (input_index == inputs_.size()) { break; }

        auto thread_id = waiting_reusable_treads_ids_.top();

        for (TaskReusableThreadPtr_T& reusable_thread_ptr : reusable_threads_)
        {
          if (reusable_thread_ptr->GetThreadId() == thread_id)
          {
            reusable_thread_ptr->Start(inputs_[input_index++]);
            ++running_reusable_thread_count_;
          }
        }

        waiting_reusable_treads_ids_.pop();
      }
    }
  }

  void Loop()
  {
    while (!all_task_done_ && !should_stop_)
    {
      Wait();
      StartNewTasks();
    }
  }
};

} // namespace tasking

#endif // ASMLGEN_TASK_MANAGER_H
