#include "asmlgen/application/tasking/task.h"

#include "asmlgen/application/tasking/orchestrator.h"

#define SFML_STATIC
#include <SFML/Network.hpp>

#include <iostream>
#include <string>
#include <utility>

namespace tasking
{

uint64_t Task::next_id_ = 0;

Task::Task(std::string url) : url_(std::move(url)), id_(next_id_++)
{
  std::cout << "task constructed " << id_ << std::endl;
}

Task::Task(Task&& other) noexcept
{
  thread_ptr_ = std::move(other.thread_ptr_);

  url_ = std::move(other.url_);
  other.url_ = "";

  id_ = other.id_;
  other.id_ = UINT64_MAX;

  required_resource_ = other.required_resource_;
  other.required_resource_ = 0;

  bytes_ = std::move(other.bytes_);

  completion_callback_ = std::move(other.completion_callback_);
}

Task& Task::operator=(Task&& rhs) noexcept
{
  thread_ptr_ = std::move(rhs.thread_ptr_);

  url_ = std::move(rhs.url_);
  rhs.url_ = "";

  required_resource_ = rhs.required_resource_;
  rhs.required_resource_ = 0;

  id_ = rhs.id_;
  rhs.id_ = UINT64_MAX;

  bytes_ = std::move(rhs.bytes_);

  completion_callback_ = std::move(rhs.completion_callback_);

  return *this;
}

void Task::RunTask()
{
  if (!thread_ptr_)
  {
    std::cout << GetId() << " Task: Start" << std::endl;
    thread_ptr_ = std::make_unique<std::jthread>(&Task::Run, this);
  }
}

void Task::Run()
{
  std::cout << GetId() << " Task: Running " << GetId() << std::endl;
  bytes_.reserve(required_resource_);

  sf::Http http(url_);
  sf::Http::Request request("", sf::Http::Request::Get);
  sf::Http::Response response = http.sendRequest(request, sf::seconds(2));
  std::copy(response.getBody().begin(), response.getBody().end(), bytes_.begin());
  Done();
}

void Task::RunPrologue()
{
  std::cout << GetId() << " Task: Prologue" << std::endl;
  sf::Http http("http://images.cocodataset.org"); // Temporary
  sf::Http::Request request(url_, sf::Http::Request::Head);
  sf::Http::Response response = http.sendRequest(request, sf::seconds(5));

  auto content_size = response.getField("content-length");
  required_resource_ = std::stoul(content_size);
}

void Task::Done()
{
  std::cout << GetId() << " Task: Done" << std::endl;
  bytes_.clear();
  bytes_.shrink_to_fit();
  completion_callback_(required_resource_, id_);
  thread_ptr_.reset();
}

std::size_t Task::GetRequiredResources() const
{
  return required_resource_;
}

void Task::SetCompletionCallback(std::function<void(std::size_t, uint64_t)>&& completion_callback)
{
  completion_callback_ = std::move(completion_callback);
}

void Task::SetUrl(std::string url)
{
  url_ = std::move(url);
  id_ = ++next_id_;
}

uint64_t Task::GetId() const
{
  return id_;
}

} // namespace tasking