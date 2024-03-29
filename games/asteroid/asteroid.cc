#include "asteroid.h"
#include "asset_manager.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>

constexpr float DEG2RAD = 0.0174532925f;

Asteroid::Asteroid()
    : x_{static_cast<float>(std::rand() % 1200)},
      y_{static_cast<float>(std::rand() % 800)},
      angle_{static_cast<float>(std::rand() % 360)},
      speed_{static_cast<float>(std::rand() % 15 + 1)},
      scale_{(std::rand() % 8 + 1) * 0.1f},
      active_{true} {
  sprite_ = std::make_shared<sf::Sprite>(
      AssetManager::Instance().Texture("asteroids.png"));
  sprite_->setPosition(x_, y_);
  sprite_->setTextureRect(sf::IntRect(0, 0, 128, 128));
  sprite_->setOrigin(64, 64);
  sprite_->setScale(scale_, scale_);
  sprite_->setRotation(angle_);

  const sf::Vector2i frameSize{128, 128};
  std::vector<sf::IntRect> v{};
  int row = 4 * (rand() % 2);
  for (int i = row; i < row + 4; ++i)
    for (int j = 0; j < 8; ++j)
      v.emplace_back(frameSize.x * j, frameSize.y * i, frameSize.x,
                     frameSize.y);
  v.pop_back();

  Animation a{"Asteroid_1", sf::seconds(1), true, v};
  animator_ = std::make_shared<Animator>(sprite_, std::move(a));
}

void Asteroid::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  target.draw(*sprite_, states);
}

void Asteroid::Update(sf::Time elapsed) {
  animator_->Update(elapsed);

  x_ += std::sin(angle_ * DEG2RAD) * 0.2;
  y_ += -std::cos(angle_ * DEG2RAD) * 0.2;

  sprite_->setPosition(x_, y_);
}

sf::FloatRect Asteroid::Bounds() const {
  return sprite_->getGlobalBounds();
}

void Asteroid::SetActive(bool active) {
  active_ = active;
}

bool Asteroid::IsActive() const {
  return active_ == true;
}

const sf::Sprite& Asteroid::Sprite() const {
  return *sprite_;
}