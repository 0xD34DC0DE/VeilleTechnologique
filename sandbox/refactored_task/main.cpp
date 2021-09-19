#include "asmlgen/application/tasking/refactor/task.h"

#include <string>
#include <vector>

using namespace tasking;

class DownloadTask : public Task<bool, long>
{
public:
  void Work(std::string&) {}
};

class DecompressTask : public Task<long, char>
{
public:
  void Work(std::vector<int>&) {}
};

class WriteTask : public Task<char, unsigned int>
{
public:
  void Work(int&) {}
};

int main(int, char**)
{

  auto a = DownloadTask {};

  a.Then(DecompressTask {});
}