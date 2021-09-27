#ifndef ASMLGEN_FILE_WRITE_DATA_H
#define ASMLGEN_FILE_WRITE_DATA_H

#include <string>
#include <vector>

namespace pipeline
{

class FileWriteData
{
public:
  FileWriteData() = default;
  FileWriteData(std::string directory_path, std::string filename, std::vector<uint8_t> data);

  [[nodiscard]] const std::string& GetDirectoryPath() const noexcept;

  [[nodiscard]] const std::string& GetFilename() const noexcept;

  [[nodiscard]] std::string GetFullPath() const;

  [[nodiscard]] const std::vector<uint8_t>& GetData() const noexcept;

  [[nodiscard]] const uint8_t* GetDataPtr() const noexcept;

  [[nodiscard]] std::size_t GetDataSize() const noexcept;

private:
  std::string directory_path_;
  std::string filename_;
  std::vector<uint8_t> data_;
};

} // namespace pipeline

#endif // ASMLGEN_FILE_WRITE_DATA_H
