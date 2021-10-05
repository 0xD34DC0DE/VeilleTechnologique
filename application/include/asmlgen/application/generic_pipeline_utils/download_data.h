#ifndef ASMLGEN_DOWNLOAD_DATA_H
#define ASMLGEN_DOWNLOAD_DATA_H

#include <string>
#include <utility>
#include <vector>

namespace pipeline
{

class DownloadData
{
public:
  DownloadData() = default;
  DownloadData(std::string url, std::vector<std::string> write_filepaths);

  [[nodiscard]] const std::string& GetUrl() const noexcept;

  [[nodiscard]] const std::string& GetHost() const noexcept;

  [[nodiscard]] const std::string& GetResource() const noexcept;

  [[nodiscard]] std::size_t GetDownloadSize() const noexcept;

  [[nodiscard]] const std::vector<std::string>& GetOutputFilepaths() const noexcept;

  [[nodiscard]] const std::vector<uint8_t>& GetData() const noexcept;

  [[nodiscard]] std::vector<uint8_t>&& GetDataRvalRef() noexcept;

  void SetDownloadSize(std::size_t download_size) noexcept;

  void SetData(const std::vector<uint8_t>& data);

  void SetData(std::vector<uint8_t>&& data) noexcept;

  void MarkFailed() noexcept;

  bool HasFailed() const noexcept;

private:
  std::string url_;
  std::string host_;
  std::string resource_;
  std::size_t download_size_;
  std::vector<std::string> output_filepaths_;

  std::vector<uint8_t> data_;

  bool failed_;

  void SetHostAndResource(const std::string& url);
};

} // namespace pipeline
#endif // ASMLGEN_DOWNLOAD_DATA_H
