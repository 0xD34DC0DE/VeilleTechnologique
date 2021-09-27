#include <iostream>

#include "asmlgen/application/coco_pipeline/coco_json_parser.h"
#include "asmlgen/application/dataset/file_reader.h"
#include "asmlgen/application/tasking/task_manager.h"

#include "asmlgen/application/generic_pipeline_utils/pipeline.h"

#define SFML_STATIC
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include <memory>
#include <thread>

using namespace tasking;
using namespace dataset;
using namespace pipeline;

int main(int, char**)
{

  std::cout << "Loading json file" << std::endl;
  std::vector<char> json =
    FileReader::ReadFile(R"(C:\Users\0xD34DC0DE\PycharmProjects\PersonSegmentation\instances_val2017.json)");
  std::cout << "Json loaded: " << json.size() << " bytes" << std::endl;

  std::cout << "Loading entries" << std::endl;

  std::unique_ptr<CocoJsonParser> coco_json_parser_ptr = std::make_unique<CocoJsonParser>();
  uint64_t entry_count = coco_json_parser_ptr->LoadJson(json);

  std::cout << "Entry loaded: " << entry_count << std::endl;

  std::vector<DownloadData> download_tasks;

  download_tasks.reserve(coco_json_parser_ptr->Entries().size());
  for (const auto& transient_entry : coco_json_parser_ptr->Entries())
  {
    download_tasks.emplace_back(transient_entry.GetURL(), "C:/Users/0xD34DC0DE/Pictures/ASMLGen/output_test");
  }

  bool is_done = false;

  TaskManager task_manager = TaskManager<FetchDownloadSize, DownloadFile, DownloadDataToWriteDataConverter, WriteFile>(
    [&is_done](bool) { is_done = true; }, 0);
  task_manager.SetInputs(download_tasks);
  task_manager.Start();

  // while (!task_manager.AllTasksDone()) {}
  std::mutex mtx {};
  std::unique_lock lock(mtx);
  std::condition_variable condition_variable;
  // Wait for task manager to be done instead of polling
  condition_variable.wait(lock, [&is_done] { return is_done; });

  //  for (const auto r : task_manager.GetResults())
  //  {
  //    std::cout << "Wrote: " << r << " bytes to disk" << std::endl;
  //  }

  auto* window = new sf::RenderWindow(sf::VideoMode(512, 512), "Hello world");

  sf::Event event {};

  while (window->isOpen())
  {
    while (window->pollEvent(event))
    {
      switch (event.type)
      {
      case sf::Event::Closed: window->close(); break;
      default: break;
      }
    }
  }

  return 0;
}