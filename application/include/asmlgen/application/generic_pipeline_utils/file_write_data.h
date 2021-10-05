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
  FileWriteData(std::vector<std::string> directory_path,
    std::string filename,
    std::vector<std::vector<uint8_t>> datas,
    bool is_decompression = false);

  [[nodiscard]] const std::vector<std::string>& GetDirectoryPaths() const noexcept;

  [[nodiscard]] const std::string& GetFilename() const noexcept;

  [[nodiscard]] const std::vector<std::vector<uint8_t>>& GetDatas() const noexcept;

  [[nodiscard]] bool IsDecompression() const;

private:
  std::vector<std::string> directory_paths_;
  std::string filename_;
  std::vector<std::vector<uint8_t>> datas_;
  bool is_decompression_;
};

} // namespace pipeline

#endif // ASMLGEN_FILE_WRITE_DATA_H
