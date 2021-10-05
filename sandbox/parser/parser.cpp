#include <iostream>

#include "asmlgen/application/coco_pipeline/coco_json_parser.h"
#include "asmlgen/application/dataset/file_reader.h"
#include "asmlgen/application/tasking/task_manager.h"

#include "asmlgen/application/coco_pipeline/coco_decompression_data.h"
#include "asmlgen/application/coco_pipeline/coco_decompression_data_to_file_write_data_converter.h"
#include "asmlgen/application/coco_pipeline/coco_transient_entry_to_coco_decompression_data_converter.h"
#include "asmlgen/application/coco_pipeline/encode_image.h"
#include "asmlgen/application/coco_pipeline/mask_decompression.h"

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
    FileReader::ReadFile(R"(C:\Users\0xD34DC0DE\PycharmProjects\PersonSegmentation\instances_train2017.json)");
  std::cout << "Json loaded: " << json.size() << " bytes" << std::endl;

  std::cout << "Loading entries" << std::endl;

  std::vector<std::string> classes { "dog", "cat" };

  std::unique_ptr<CocoJsonParser> coco_json_parser_ptr = std::make_unique<CocoJsonParser>();
  uint64_t entry_count =
    coco_json_parser_ptr->LoadJson(json, classes, "C:/Users/0xD34DC0DE/Pictures/ASMLGen/output_test");

  std::cout << "Entry loaded: " << entry_count << std::endl;

  std::vector<TransientCocoEntry> sample = std::vector<TransientCocoEntry>(
    coco_json_parser_ptr->Entries().begin(), coco_json_parser_ptr->Entries().begin() + 200);

  std::vector<DownloadData> download_tasks;

  download_tasks.reserve(sample.size());
  for (const auto& transient_entry : sample)
  {
    const std::string output_directory = "C:/Users/0xD34DC0DE/Pictures/ASMLGen/output_test/";
    std::vector<std::string> output_paths;
    for (const auto& annotations : transient_entry.AnnotationsData())
    {
      output_paths.emplace_back(output_directory + annotations.first);
    }
    download_tasks.emplace_back(transient_entry.GetURL(), output_paths);
  }

  bool is_done = false;
  std::mutex mtx {};
  std::unique_lock lock(mtx);
  std::condition_variable condition_variable;

  auto done_callback = [&is_done, &condition_variable](bool)
  {
    is_done = true;
    condition_variable.notify_one();
  };
  size_t total_items = sample.size();
  // TODO make progression callback accept -> number_done, total_items, percentage
  // TODO add task retry
  auto progress_callback = [&total_items](size_t items_done)
  { std::cout << "\r          \r" << items_done << "/" << total_items << std::flush; };

  TaskManager task_manager_image =
    TaskManager<FetchDownloadSize, DownloadFile, DownloadDataToWriteDataConverter, WriteFile>(
      done_callback, progress_callback, 0);
  task_manager_image.SetInputs(download_tasks);
  task_manager_image.Start();

  // while (!task_manager.AllTasksDone()) {}

  std::cout << "Downloading done, generating masks" << std::endl;

  // Wait for task manager to be done instead of polling
  condition_variable.wait(lock, [&is_done] { return is_done; });

  is_done = false;
  std::vector<TransientCocoEntry> transient_entries = sample;

  TaskManager task_manager_masks = TaskManager<CocoTransientEntryToCocoDecompressionDataConverter,
    MaskDecompression,
    EncodeImage,
    CocoDecompressionDataToFileWriteDataConverter,
    WriteFile>(done_callback, progress_callback, 0);

  task_manager_masks.SetInputs(transient_entries);
  task_manager_masks.Start();

  condition_variable.wait(lock, [&is_done] { return is_done; });

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