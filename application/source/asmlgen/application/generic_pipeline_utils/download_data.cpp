#include "asmlgen/application/generic_pipeline_utils/download_data.h"

namespace pipeline
{

DownloadData::DownloadData(std::string url, std::vector<std::string> write_filepaths)
  : url_(std::move(url)), output_filepaths_(std::move(write_filepaths)), failed_(false)
{
  SetHostAndResource(url_);
}

const std::string& DownloadData::GetUrl() const noexcept
{
  return url_;
}

const std::string& DownloadData::GetHost() const noexcept
{
  return host_;
}

const std::string& DownloadData::GetResource() const noexcept
{
  return resource_;
}

std::size_t DownloadData::GetDownloadSize() const noexcept
{
  return download_size_;
}

const std::vector<std::string>& DownloadData::GetOutputFilepaths() const noexcept
{
  return output_filepaths_;
}

const std::vector<uint8_t>& DownloadData::GetData() const noexcept
{
  return data_;
}

void DownloadData::SetDownloadSize(std::size_t download_size) noexcept
{
  download_size_ = download_size;
}

void DownloadData::SetData(const std::vector<uint8_t>& data)
{
  data_ = data;
}

void DownloadData::SetData(std::vector<uint8_t>&& data) noexcept
{
  data_ = std::move(data);
}

void DownloadData::SetHostAndResource(const std::string& url)
{
  std::size_t protocol_end = url.find_first_of(':') + 3; // start of host is after "://" -> +3
  std::size_t host_end = url.find_first_of('/', protocol_end);

  host_ = url.substr(0, host_end);
  resource_ = url.substr(host_end);
}

std::vector<uint8_t>&& DownloadData::GetDataRvalRef() noexcept
{
  return std::move(data_);
}

void DownloadData::MarkFailed() noexcept
{
  failed_ = true;
}

bool DownloadData::HasFailed() const noexcept
{
  return failed_;
}

} // namespace pipeline