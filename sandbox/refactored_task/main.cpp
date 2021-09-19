#include "asmlgen/application/tasking/refactor/task.h"

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
  using type = typename std::tuple_element<sizeof...(Types) - 1, std::tuple<Types...>>::type::InputType;
};

template<typename Type>
typename Type::OutputType CallNextRecursive(typename Type::InputType input, Type next)
{
  return next.Apply(input);
}

template<typename Type, typename... Types>
typename std::enable_if<(sizeof...(Types) > 0), typename OutputTypeList<Type, Types...>::type>::type CallNextRecursive(
  typename InputTypeList<Type, Types...>::type input, Type next, Types... nexts)
{
  auto output = next.Apply(input);
  return CallNextRecursive(output, nexts...);
}

int main(int, char**)
{

  //  auto a = DownloadTask {};
  //
  //  a.Then(DecompressTask {});
}