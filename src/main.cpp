#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window.hpp>

using namespace sf;

int main() {

  const int width = 100;
  const int height = 100;

  Image image({width, height}, Color::Black);

  Texture texture;
  [[maybe_unused]] bool ok = texture.loadFromImage(image);

  Sprite sprite(texture);

  Clock clock;
  float elapsedTime = 0.f;

  unsigned int i = 0, j = 0;

  RenderWindow window(VideoMode({width, height}), "My simulation");

  while (window.isOpen()) {
    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }
    }

    elapsedTime += clock.restart().asMilliseconds();

    if (elapsedTime >= 1.f) {
      elapsedTime = 0.f;

      image.setPixel({i, j}, Color::White);
      if (++i == width) {
        i = 0;
        if (++j == height) {
          j = 0;
        }
      }
    }

    texture.update(image);
    window.clear();
    window.draw(sprite);
    window.display();
  }
}
