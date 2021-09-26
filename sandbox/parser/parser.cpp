#include <iostream>

#include "asmlgen/application/dataset/coco_json_parser.h"
#include "asmlgen/application/dataset/file_reader.h"
#include "asmlgen/application/tasking/call_chain.h"

#define SFML_STATIC
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include <memory>
#include <thread>

using namespace dataset;

int main(int, char**)
{

  std::cout << "Loading json file" << std::endl;
  std::vector<char> json =
    FileReader::ReadFile(R"(C:\Users\0xD34DC0DE\PycharmProjects\PersonSegmentation\instances_val2017.json)");
  std::cout << "Json loaded: " << json.size() << " bytes" << std::endl;

  std::cout << "Loading entries" << std::endl;

  std::unique_ptr<DatasetJsonParser<TransientCocoEntry>> coco_json_parser_ptr = std::make_unique<CocoJsonParser>();
  uint64_t entry_count = coco_json_parser_ptr->LoadJson(json);

  //  std::vector<std::jthread> threads;
  //
  //    for (int i = 0; i < 20; ++i)
  //    {
  //      std::string url = coco_json_parser_ptr->[i].GetUrl();
  //      threads.emplace_back(
  //        [] {
  //
  //        });
  //    }

  std::cout << "Entry loaded: " << entry_count << std::endl;

  sf::Http http("http://images.cocodataset.org/");
  sf::Http::Request request("/val2017/000000114907.jpg", sf::Http::Request::Get);
  sf::Http::Response response = http.sendRequest(request, sf::seconds(2));

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