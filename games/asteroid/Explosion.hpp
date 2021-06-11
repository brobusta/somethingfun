#pragma once

#include "Animator.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <memory>

#include <spdlog/spdlog.h>

class Explosion final : public sf::Drawable {
 public:
  Explosion();

 public:
  void update(sf::Time elapsed);

 private:
  void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

 private:
  float x_;
  float y_;

  SpritePtr sprite_{nullptr};
  AnimatorPtr animator_{nullptr};
};