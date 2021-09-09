#include "asmlgen/application/dataset/file_reader.h"

#include <fstream>

namespace dataset
{

std::vector<char> FileReader::ReadFile(const std::string& filepath)
{
  std::ifstream inputStream;
  inputStream.open(filepath);

  if (inputStream.is_open())
  {
    inputStream.seekg(0, std::ios_base::end);

    std::size_t charCount = inputStream.tellg();

    inputStream.seekg(0, std::ios_base::beg);

    std::vector<char> chars((charCount + 1) / sizeof(char));

    inputStream.read((char*)&chars[0], charCount);

    inputStream.close();

    chars[chars.size() - 1] = '\0';

    return chars;
  }

  return {};
}
} // namespace dataset