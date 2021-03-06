#ifndef ASMLGEN_REUSABLE_THREAD_H
#define ASMLGEN_REUSABLE_THREAD_H

#include "asmlgen/application/config/assertion.h"
#include "call_chain.h"

#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

namespace tasking
{

template<Chainable Chain>
class ReusableThread
{
public:
  ReusableThread() = default;

  ReusableThread(Chain&& chain, std::function<void(std::thread::id, typename Chain::OutputType)>&& done_callback)
    : chain_(std::move(chain)), state_(State::Waiting), done_callback_(std::move(done_callback)),
      thread_(std::jthread(&ReusableThread<Chain>::Loop, this))
  {
    stop_token_ = thread_.get_stop_token();
  };

  void Start(typename Chain::InputType input)
  {
    std::unique_lock<std::mutex> lock(mutex_);
    ASSERT(!(state_ & State::Running), "Reusable thread was started, but was already running");
    state_ = static_cast<State>(State::Running | State::Wake);
    chain_input_ = input;
    condition_variable_.notify_one();
  };

  void Terminate()
  {
    std::unique_lock<std::mutex> lock(mutex_);
    state_ = static_cast<State>(state_ | State::Stop);
    thread_.request_stop();
    condition_variable_.notify_one();
  };

  [[nodiscard]] bool IsDone()
  {
    std::scoped_lock<std::mutex> lock(mutex_);
    return state_ & State::Waiting;
  };

  [[nodiscard]] bool IsRunning()
  {
    std::scoped_lock<std::mutex> lock(mutex_);
    return state_ & State::Running;
  };

  [[nodiscard]] std::thread::id GetThreadId()
  {
    return thread_.get_id();
  }

private:
  std::jthread thread_;
  std::mutex mutex_;
  std::condition_variable_any condition_variable_;

  std::stop_token stop_token_;

  std::function<void(std::thread::id, typename Chain::OutputType)> done_callback_;

  Chain chain_;
  typename Chain::InputType chain_input_;

  enum State : uint32_t
  {
    Invalid = 0,
    Waiting = 1 << 0,
    Running = 1 << 1,
    Wake = 1 << 2,
    Stop = 1 << 3,
  } state_;

  typename Chain::OutputType RunChain()
  {
    return chain_.Start(chain_input_);
  };

  void Done(typename Chain::OutputType result)
  {
    {
      std::unique_lock<std::mutex> lock(mutex_);
      state_ = static_cast<State>(state_ & ~State::Running);
      state_ = static_cast<State>(state_ | State::Waiting);
    }
    done_callback_(thread_.get_id(), result);
  };

  void Loop()
  {
    while (!(state_ & State::Stop))
    {
      Wait();
      if (state_ & State::Stop) { break; }
      auto result = RunChain();
      Done(result);
    }
    return;
  };

  void Wait()
  {
    std::unique_lock<std::mutex> lock(mutex_);

    condition_variable_.wait(lock, stop_token_, [this]() -> bool { return state_ & (State::Wake | State::Stop); });
    state_ = static_cast<State>(state_ & ~State::Wake);
  };
};

} // namespace tasking

#endif // ASMLGEN_REUSABLE_THREAD_H

// 11:04