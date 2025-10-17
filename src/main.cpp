#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

using namespace sf;


int main(){
  RenderWindow window(VideoMode({500, 500}), "My simulation");
  
  while (window.isOpen()){

    while (const std::optional event = window.pollEvent()){
      if (event -> is<sf::Event::Closed>()){
        window.close();
      }
    }

    window.clear();
    window.display();
  }
}
