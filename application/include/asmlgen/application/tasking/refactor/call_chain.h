#ifndef ASMLGEN_CALL_CHAIN_H
#define ASMLGEN_CALL_CHAIN_H

#include <tuple>

namespace tasking
{

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

template<typename T>
concept IsChainCallable = requires(T obj)
{
  {
    obj.Call(std::declval<T::InputType>())
    } -> std::same_as<typename T::OutputType>;
};

template<typename T>
concept Chainable = requires
{
  { HasChainTypes<T> };
  { IsChainCallable<T> };
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
  constexpr void verifier_() requires Chainable<TChild> {};
};

} // namespace tasking

#endif // ASMLGEN_CALL_CHAIN_H
