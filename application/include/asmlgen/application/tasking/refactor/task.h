#ifndef ASMLGEN_TASK_H
#define ASMLGEN_TASK_H

#include <functional>
#include <memory>
#include <optional>
#include <type_traits>
#include <typeinfo>
#include <utility>

namespace tasking
{

template<template<typename Input, typename Output> class T, typename Input, typename Output>
concept Chainable = requires(T<Input, Output> task)
{
  { task.Start(Input {}) };
};

template<typename R, typename... A>
R ret(R (*)(A...));

template<typename C, typename R, typename... A>
R ret(R (C::*)(A...));

template<typename InputType, typename OutputType>
class Task
{
  // public:
  //   Task() = default;
  //   virtual ~Task() = default;
  //
  //   template<template<typename, typename> typename Task_T, typename T>
  //   Task<OutputType, T> Then(Task_T<OutputType, T> a)
  //   {
  //     f_ = [a] { Task_T<OutputType, T> v = a.Start({}); };
  //     //    next_task_ = std::move(next);
  //     //    return next_task_;
  //     return Task<OutputType, T> {};
  //   }
  //
  //   void Start(InputType input);
  //   //  {
  //   //    Work(input);
  //   //    Done();
  //   //  }
  //   //
  //   //  virtual void Work(InputType& input) = 0;
  //
  //   //  void Done()
  //   //  {
  //   //    if (next_task_.has_value()) { next_task_.value().Start(output_); }
  //   //  }
  //   //
  //   //  auto GetResultFromLast()
  //   //  {
  //   //    if (next_task_.has_value()) { return GetResultFromLast(); }
  //   //    else
  //   //    {
  //   //      return output_;
  //   //    }
  //   //  }
  //
  // protected:
  //   OutputType output_;
  //
  // private:
  //   bool has_next_ = false;
  //   std::function<void()> f_;
  //
  //   auto GetResultFromLast(Task& next)
  //   {
  //     if (next.next_task_.has_value()) { return GetResultFromLast(next.next_task_.value()); }
  //     else
  //     {
  //       output_;
  //     }
  //   }
};

} // namespace tasking

#endif // ASMLGEN_TASK_H
