#include "FluidSimulation.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window.hpp>
#include <SFML/Window/Mouse.hpp>

using namespace sf;

int main() {

  const int size = 500;
  const int scale = 10;

  Fluid f(size / scale, 0.1f, 0, 0, 4);

  Image image({size, size}, Color::Black);

  Texture texture;
  [[maybe_unused]] bool ok = texture.loadFromImage(image);

  Sprite sprite(texture);

  // Clock clock;
  // float elapsedTime = 0.f;
  RenderWindow window(VideoMode({size, size}), "My simulation");

  while (window.isOpen()) {
    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }
      if (sf::Mouse::isButtonPressed(Mouse::Button::Left)) {
        Vector2i mousePos = Mouse::getPosition(window);
        f.addDensity(mousePos.x / 10, mousePos.y / 10, 100);
      }
    }

    f.step();
    for (unsigned int i = 0; i < size; i++) {
      for (unsigned int j = 0; j < size; j++) {
        int d = f.getDensities(i / scale, j / scale);
        image.setPixel({i, j}, Color(255, 255, 255, 255 * d));
      }
    }
    // elapsedTime += clock.restart().asMilliseconds();

    // if (elapsedTime >= .01f) {
    // elapsedTime = 0.f;
    //}

    texture.update(image);
    window.clear();
    window.draw(sprite);
    window.display();
  }
}
