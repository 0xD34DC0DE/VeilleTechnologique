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

template<typename... Types>
struct InputTypeList
{
  using type = typename std::tuple_element<0, std::tuple<Types...>>::type::InputType;
};

template<typename... Types>
struct OutputTypeList
{
  using type = typename std::tuple_element<sizeof...(Types) - 1, std::tuple<Types...>>::type::OutputType;
};

template<typename Type>
typename Type::OutputType CallNextRecursive(typename Type::InputType input, Type next)
{
  return next.Call(input);
}

template<typename Type, typename... Types>
typename std::enable_if<(sizeof...(Types) > 0), typename OutputTypeList<Type, Types...>::type>::type CallNextRecursive(
  typename InputTypeList<Type, Types...>::type input, Type next, Types... nexts)
{
  auto output = next.Call(input);
  return CallNextRecursive(output, nexts...);
}

template<typename InType, typename TransformerTuple, std::size_t... Int>
constexpr auto ChainCallHelper(InType& in, TransformerTuple& tranformer_tuple, std::index_sequence<Int...>)
{
  return CallNextRecursive(in, std::get<Int>(tranformer_tuple)...);
}

template<typename InType, typename TranformerTuple>
constexpr auto ChainCallTuple(InType& in, TranformerTuple& tranformer_tuple)
{
  return ChainCallHelper(in, tranformer_tuple, std::make_index_sequence<std::tuple_size<TranformerTuple>::value> {});
}

template<typename... Calls>
struct ChainedCalls
{
  using InputType = typename InputTypeList<Calls...>::type;
  using OutputType = typename OutputTypeList<Calls...>::type;

  std::tuple<Calls...> calls;

  explicit ChainedCalls(Calls... calls) : calls(std::tuple<Calls...>(calls...)) {};

  OutputType Call(InputType input)
  {
    return ChainCallTuple(input, calls);
  }
};

template<typename... Calls>
auto MakeChainedCalls(Calls... calls)
{
  return ChainedCalls<Calls...>(calls...);
}

struct callable1
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

struct callable2
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
constexpr auto CallNext(typename Chainable::InputType& input, Chainable function)
{
  return function.Call(input);
}

template<typename Chainable, typename... Chainables>
constexpr auto CallNext(typename Chainable::InputType& input, Chainable function, Chainables... next_functions)
{
  auto output = function.Call(input);
  CallNext(output, next_functions...);
}

template<typename InputType, typename ChainableTuple, std::size_t... Indices>
constexpr auto UnpackChainableTupleAndStartChain(
  InputType input, ChainableTuple& chainable_tuple, std::index_sequence<Indices...>)
{
  return CallNext(input, std::get<Indices...>(chainable_tuple));
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

  OutputType Start(InputType input)
  {
    return StartChain(input, functions);
  }

  std::tuple<Chainables...> functions;
};

int main(int, char**)
{
  auto f1 = callable1();
  auto f2 = callable2();
  auto chain = MakeChainedCalls(f1, f2);

  std::cout << chain.Call(true) << std::endl;
  std::cout << chain.Call(false) << std::endl;
}