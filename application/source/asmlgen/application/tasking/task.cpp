#include "asmlgen/application/tasking/task.h"

#include "asmlgen/application/tasking/orchestrator.h"

#include <SFML/Network.hpp>

#include <string>
#include <utility>

namespace tasking
{

Task::Task(std::string url) : url_(std::move(url)), required_resource_(0) {}

Task::Task(Task&& other) noexcept
{
  thread_ptr_ = std::move(other.thread_ptr_);

  url_ = std::move(other.url_);
  other.url_ = "";

  required_resource_ = other.required_resource_;
  other.required_resource_ = 0;
}

Task& Task::operator=(Task&& rhs) noexcept
{
  thread_ptr_ = std::move(rhs.thread_ptr_);

  url_ = std::move(rhs.url_);
  rhs.url_ = "";

  required_resource_ = rhs.required_resource_;
  rhs.required_resource_ = 0;

  return *this;
}

void Task::RunTask()
{
  if (!thread_ptr_) { thread_ptr_ = std::make_unique<std::jthread>(&Task::Run, this); }
}

void Task::Run() {}

void Task::RunPrologue()
{

  sf::Http http(url_);
  sf::Http::Request request("", sf::Http::Request::Head);
  sf::Http::Response response = http.sendRequest(request, sf::seconds(2));

  required_resource_ = std::stoul(response.getField("Content-Length"));
}

void Task::Done()
{
  completion_callback_(required_resource_);
}

std::size_t Task::GetRequiredResources() const
{
  return required_resource_;
}

void Task::SetCompletionCallback(std::function<void(std::size_t)>&& completion_callback)
{
  completion_callback_ = std::move(completion_callback);
}

void Task::SetUrl(std::string url)
{
  url_ = std::move(url);
}

} // namespace tasking