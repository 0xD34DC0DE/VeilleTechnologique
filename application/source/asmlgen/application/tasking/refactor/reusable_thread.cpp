#include "asmlgen/application/tasking/refactor/reusable_thread.h"

#include "asmlgen/application/config/assertion.h"

namespace tasking
{
//
// template<Chainable Chain>
// ReusableThread<Chain>::ReusableThread(Chain&& chain) : chain_(std::move(chain))
//{}
//
// template<Chainable Chain>
// void ReusableThread<Chain>::SetDoneCallback(std::function<void(typename Chain::OutputType)>&& done_callback)
//{
//  done_callback_ = std::move(done_callback);
//}
//
// template<Chainable Chain>
// bool ReusableThread<Chain>::IsRunning()
//{
//  std::scoped_lock<std::mutex> lock(mutex_);
//  return state_ & State::Running;
//}
//
// template<Chainable Chain>
// bool ReusableThread<Chain>::IsDone()
//{
//  std::scoped_lock<std::mutex> lock(mutex_);
//  return state_ & State::Waiting;
//}
//
// template<Chainable Chain>
// void ReusableThread<Chain>::Start(typename Chain::InputType input)
//{
//  std::unique_lock<std::mutex> lock(mutex_);
//  ASSERT(!(state_ & State::Running), "Reusable thread was started, but was already running");
//  state_ = State::Running | State::Wake;
//  chain_input_ = input;
//  condition_variable_.notify_one();
//}
//
// template<Chainable Chain>
// void ReusableThread<Chain>::Loop()
//{
//  while (!(state_ & State::Stop))
//  {
//    Wait();
//    RunChain();
//    Done();
//  }
//}
//
// template<Chainable Chain>
// void ReusableThread<Chain>::Terminate()
//{
//  std::scoped_lock<std::mutex> lock(mutex_);
//  state_ |= State::Stop;
//}
//
// template<Chainable Chain>
// void ReusableThread<Chain>::Wait()
//{
//  std::unique_lock<std::mutex> lock(mutex_);
//
//  condition_variable_.wait(lock, stop_token_, [this]() -> bool { return state_ & State::Wake; });
//  state_ &= ~State::Wake;
//}
//
// template<Chainable Chain>
// typename Chain::OutputType ReusableThread<Chain>::RunChain()
//{
//  return chain_.StartChain(chain_input_);
//}
// template<Chainable Chain>
// void ReusableThread<Chain>::Done(typename Chain::OutputType result)
//{
//  {
//    std::unique_lock<std::mutex> lock(mutex_);
//    state_ &= ~State::Running;
//    state_ |= State::Waiting;
//  }
//  done_callback_(result);
//}

} // namespace tasking