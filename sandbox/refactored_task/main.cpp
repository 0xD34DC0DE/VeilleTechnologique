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

// struct chainable1 : MakeChainObj<chainable1, std::string, bool>
//{
//   std::string Call(bool b)
//   {
//     if (b) { return "1"; }
//     else
//     {
//       return "A";
//     }
//   }
// };
//
// struct chainable2 : MakeChainObj<chainable1, int, std::string>
//{
//   int Call(std::string s)
//   {
//     if (s == "1") { return 1; }
//     else
//     {
//       return 0;
//     }
//   }
// };
//
// int main(int, char**)
//{
//   auto f1 = chainable1();
//   auto f2 = chainable2();
//   f1.verifier_();
//
//   auto chain = MakeChained(f1, f2);
//
//   std::cout << chain.Start(true) << std::endl;
//   std::cout << chain.Start(false) << std::endl;
// }

// 9:17 to 12:00