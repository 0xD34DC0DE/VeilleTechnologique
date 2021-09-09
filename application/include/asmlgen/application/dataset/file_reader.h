#ifndef ASMLGEN_FILE_READER_H
#define ASMLGEN_FILE_READER_H

#include <string>
#include <vector>

namespace dataset
{

// TODO replace vector<char> by a stream if files get to big for memory
class FileReader
{
public:
  static std::vector<char> ReadFile(const std::string& filepath);
};

} // namespace dataset

#endif // ASMLGEN_FILE_READER_H
