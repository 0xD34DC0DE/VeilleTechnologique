#include "asmlgen/application/tasking/refactor/task.h"

#include <iostream>
#include <string>
#include <vector>

using namespace tasking;

// class DownloadTask : public Task<bool, long>
//{
// public:
//   void Work(std::string&) {}
// };
//
// class DecompressTask : public Task<long, char>
//{
// public:
//   void Work(std::vector<int>&) {}
// };
//
// class WriteTask : public Task<char, unsigned int>
//{
// public:
//   void Work(int&) {}
// };

// REFACTOR

template<typename... ChainObjs>
struct GetOutputTypeOfLast
{
  using OutputType = typename std::tuple_element<sizeof...(ChainObjs) - 1, std::tuple<ChainObjs...>>::type::OutputType;
};

template<typename... ChainObjs>
struct GetInputTypeOfFirst
{
  using InputType = typename std::tuple_element<0, std::tuple<ChainObjs...>>::type::InputType;
};

template<typename ChainObj>
constexpr typename ChainObj::OutputType CallNext(typename ChainObj::InputType& input, ChainObj function)
{
  return function.Call(input);
}

template<typename ChainObj>
constexpr typename ChainObj::OutputType EndChain(typename ChainObj::InputType& input, ChainObj function)
{
  return function.Call(input);
}

template<typename ChainObj, typename... ChainObjs>
constexpr typename GetOutputTypeOfLast<ChainObj, ChainObjs...>::OutputType CallNext(
  typename ChainObj::InputType& input, ChainObj function, ChainObjs... next_functions)
{
  auto output = function.Call(input);

  if constexpr (sizeof...(ChainObjs) > 0) { return CallNext(output, next_functions...); }
  else
  {
    return EndChain(output, next_functions...);
  }
}

template<typename InputType, typename ChainObjTuple, std::size_t... Indices>
constexpr auto UnpackChainObjTupleAndStartChain(
  InputType input, ChainObjTuple& chainable_tuple, std::index_sequence<Indices...>)
{
  return CallNext(input, std::get<Indices>(chainable_tuple)...);
}

template<typename InputType, typename ChainObjTuple>
constexpr auto StartChain(InputType input, ChainObjTuple& chainable_tuple)
{
  return UnpackChainObjTupleAndStartChain(
    input, chainable_tuple, std::make_index_sequence<std::tuple_size<ChainObjTuple>::value> {});
}

template<typename... ChainObjs>
struct ChainedObjs
{

  using InputType = typename GetInputTypeOfFirst<ChainObjs...>::InputType;
  using OutputType = typename GetOutputTypeOfLast<ChainObjs...>::OutputType;

  explicit ChainedObjs(ChainObjs... chainables) : functions(chainables...) {};

  std::tuple<ChainObjs...> functions;

  OutputType Start(InputType input)
  {
    return StartChain(input, functions);
  }
};

template<typename T>
concept HasChainTypes = requires(T obj)
{
  { T::InputType };
  { T::OutputType };
};

// template<class...>
// struct types
//{
//   using type = types;
// };
//
// template<class Sig>
// struct args;
// template<class R, class... Args>
// struct args<R(Args...)> : types<Args...>
//{};
// template<class Sig>
// using args_t = typename args<Sig>::type;

template<typename T>
concept IsChainCallable = requires(T obj)
{
  {
    obj.Call(std::declval<T::InputType>())
    } -> std::same_as<typename T::OutputType>;
};

template<typename T>
concept IsChainable = requires
{
  HasChainTypes<T>&& IsChainCallable<T>;
};

template<typename... ChainObjs>
constexpr auto MakeChained(ChainObjs... chainables)
{
  return ChainedObjs(chainables...);
}

template<typename TChild, typename Output_T, typename Input_T>
// requires IsChainCallable<TChild>
struct MakeChainObj
{

  using InputType = Input_T;
  using OutputType = Output_T;

  // private:
  constexpr void verifier_() requires IsChainable<TChild> {};
};

struct chainable1 : MakeChainObj<chainable1, std::string, bool>
{
  std::string Call(bool b)
  {
    if (b) { return "1"; }
    else
    {
      return "A";
    }
  }
};

struct chainable2 : MakeChainObj<chainable1, int, std::string>
{
  int Call(std::string s)
  {
    if (s == "1") { return 1; }
    else
    {
      return 0;
    }
  }
};

int main(int, char**)
{
  auto f1 = chainable1();
  auto f2 = chainable2();
  f1.verifier_();

  auto chain = MakeChained(f1, f2);

  std::cout << chain.Start(true) << std::endl;
  std::cout << chain.Start(false) << std::endl;
}

// 9:17 to 12:00