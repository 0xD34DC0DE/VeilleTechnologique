#include "asmlgen/application/tasking/task_manager.h"

#include "asmlgen/application/config/assertion.h"

#define SFML_STATIC
#include <SFML/Network.hpp>

#include <asmlgen/application/generic_pipeline_utils/download_data.h>
#include <asmlgen/application/generic_pipeline_utils/download_data_to_write_data_converter.h>
#include <asmlgen/application/generic_pipeline_utils/download_file.h>
#include <asmlgen/application/generic_pipeline_utils/fetch_download_size.h>
#include <asmlgen/application/generic_pipeline_utils/write_file.h>

#include <filesystem>
#include <fstream>
#include <iostream>

using namespace tasking;
using namespace pipeline;

int main(int, char**)
{
  std::string output_directory_path = "C:/Users/0xD34DC0DE/Pictures/ASMLGen/output_test";
  std::vector<std::string> urls { "http://images.cocodataset.org/val2017/000000114907.jpg",
    "http://images.cocodataset.org/val2017/000000115245.jpg",
    "http://images.cocodataset.org/val2017/000000437898.jpg",
    "http://images.cocodataset.org/val2017/000000470952.jpg",
    "http://images.cocodataset.org/val2017/000000481413.jpg",
    "http://images.cocodataset.org/val2017/000000451150.jpg",
    "http://images.cocodataset.org/val2017/000000149222.jpg",
    "http://images.cocodataset.org/val2017/000000507081.jpg" };

  std::vector<DownloadData> download_tasks;

  download_tasks.reserve(urls.size());
  for (const auto& url : urls)
  {
    download_tasks.emplace_back(url, output_directory_path);
  }

  TaskManager task_manager =
    TaskManager<FetchDownloadSize, DownloadFile, DownloadDataToWriteDataConverter, WriteFile>(2);
  task_manager.SetInputs(download_tasks);
  task_manager.Start();

  while (!task_manager.AllTasksDone()) {}

  for (const auto r : task_manager.GetResults())
  {
    std::cout << "Wrote: " << r << " bytes to disk" << std::endl;
  }
}

// 9:17 to 12:00