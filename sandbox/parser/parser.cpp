#include <iostream>

#include "asmlgen/application/dummy.h"

#include <SFML/Graphics.hpp>

int main(int, char**) {
  std::cout << "Project is compiling!" << std::endl;

  auto* window = new sf::RenderWindow(sf::VideoMode(512, 512), "Hello world");

  sf::Event event{};

  while(window->isOpen()) {
    while(window->pollEvent(event)) {
      switch(event.type) {
      case sf::Event::Closed:
        window->close();
        break;
        default:
          break;
      }
    }
  }

  return 0;
}