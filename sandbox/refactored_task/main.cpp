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

template<typename... Chainables>
struct GetOutputTypeOfLast
{
  using OutputType =
    typename std::tuple_element<sizeof...(Chainables) - 1, std::tuple<Chainables...>>::type::OutputType;
};

template<typename... Chainables>
struct GetInputTypeOfFirst
{
  using InputType = typename std::tuple_element<0, std::tuple<Chainables...>>::type::InputType;
};

template<typename Chainable>
constexpr typename Chainable::OutputType CallNext(typename Chainable::InputType& input, Chainable function)
{
  return function.Call(input);
}

template<typename Chainable>
constexpr typename Chainable::OutputType EndChain(typename Chainable::InputType& input, Chainable function)
{
  return function.Call(input);
}

template<typename Chainable, typename... Chainables>
constexpr typename GetOutputTypeOfLast<Chainable, Chainables...>::OutputType CallNext(
  typename Chainable::InputType& input, Chainable function, Chainables... next_functions)
{
  auto output = function.Call(input);

  if constexpr (sizeof...(Chainables) > 0) { return CallNext(output, next_functions...); }
  else
  {
    return EndChain(output, next_functions...);
  }
}

template<typename InputType, typename ChainableTuple, std::size_t... Indices>
constexpr auto UnpackChainableTupleAndStartChain(
  InputType input, ChainableTuple& chainable_tuple, std::index_sequence<Indices...>)
{
  return CallNext(input, std::get<Indices>(chainable_tuple)...);
}

template<typename InputType, typename ChainableTuple>
constexpr auto StartChain(InputType input, ChainableTuple& chainable_tuple)
{
  return UnpackChainableTupleAndStartChain(
    input, chainable_tuple, std::make_index_sequence<std::tuple_size<ChainableTuple>::value> {});
}

template<typename... Chainables>
struct Chained
{

  using InputType = typename GetInputTypeOfFirst<Chainables...>::InputType;
  using OutputType = typename GetOutputTypeOfLast<Chainables...>::OutputType;

  explicit Chained(Chainables... chainables) : functions(chainables...) {};

  std::tuple<Chainables...> functions;

  OutputType Start(InputType input)
  {
    return StartChain(input, functions);
  }
};

template<typename... Chainables>
constexpr auto MakeChained(Chainables... chainables)
{
  return Chained(chainables...);
}

struct chainable1
{
  std::string Call(bool b)
  {
    if (b) { return "1"; }
    else
    {
      return "A";
    }
  }

  using InputType = bool;
  using OutputType = std::string;
};

struct chainable2
{
  int Call(std::string s)
  {
    if (s == "1") { return 1; }
    else
    {
      return 0;
    }
  }

  using InputType = std::string;
  using OutputType = int;
};

int main(int, char**)
{
  auto f1 = chainable1();
  auto f2 = chainable2();
  auto chain = MakeChained(f1, f2);

  std::cout << chain.Start(true) << std::endl;
  std::cout << chain.Start(false) << std::endl;
}

// 9:17