#ifndef ASMLGEN_TASK_H
#define ASMLGEN_TASK_H

#include <functional>
#include <thread>

namespace tasking
{

// TODO make task accept lambdas to run instead of hardcoded Run() and RunPrologue()
class Orchestrator;

class Task
{
public:
  Task() = default;
  explicit Task(std::string url);

  Task(Task&& other) noexcept;
  Task& operator=(Task&& rhs) noexcept;

  Task(const Task&) = delete;
  Task& operator=(const Task&) = delete;

  ~Task() = default;

  void RunTask();

  [[nodiscard]] std::size_t GetRequiredResources() const;

  void SetCompletionCallback(std::function<void(std::size_t)>&& completion_callback);

private:
  std::string url_;
  std::size_t required_resource_ {};
  std::unique_ptr<std::jthread> thread_ptr_;

  // TODO Optimise to reference of function that will be created when constructing the orchestrator to avoid recreating
  // std::function's
  std::function<void(std::size_t)> completion_callback_;

  void Run();

  ///
  /// Work to be executed before Run()
  ///
  /// @note Not managed by the orchestrator since it shouldn't use resources
  ///
  void RunPrologue();

  void Done();
};

} // namespace tasking

#endif // ASMLGEN_TASK_H
