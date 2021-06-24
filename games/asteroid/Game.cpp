#include "Game.hpp"

#include "AssetManager.hpp"
#include "Collision.hpp"

#include <spdlog/spdlog.h>
#include <cstdlib>

Game::Game()
    : window_(sf::VideoMode(1200, 800), "Asteroid v1"),
      score_{},
      spaceShip_{},
      asteroids_{rand() % 90 + 10},
      explosions_{},
      bullets_{},
      shouldRenderBounds_{false} {
  score_.setFont(AssetManager::getInstance().GetFont("EvilEmpire-4BBVK.ttf"));
  score_.setPosition(10, 10);
  score_.setString("Score: 0");
  score_.setCharacterSize(32);
  score_.setFillColor(sf::Color::Red);
}

void Game::run(int fps) {
  sf::Clock clock{};
  sf::Time time_since_last_update{sf::Time::Zero};
  sf::Time time_per_frame = sf::seconds(1.f / static_cast<float>(fps));

  while (window_.isOpen()) {
    this->processEvents();

    bool repaint{false};
    time_since_last_update += clock.restart();
    while (time_since_last_update > time_per_frame) {
      repaint = true;
      time_since_last_update -= time_per_frame;
      this->update(time_per_frame);
    }

    if (repaint) {
      this->render();
    }
  }
}

void Game::processEvents() {
  sf::Event event;
  while (window_.pollEvent(event)) {
    switch (event.type) {
      case sf::Event::EventType::Closed:
        window_.close();
        break;
      case sf::Event::EventType::KeyPressed:
        if (event.key.code == sf::Keyboard::Space) {
          handleSpaceshipFire();
        } else if (event.key.code == sf::Keyboard::R) {
          handleReset();
        } else if (event.key.code == sf::Keyboard::B) {
          shouldRenderBounds_ = !shouldRenderBounds_;
        }

        break;
      default:
        break;
    }
  }
}

void Game::update(sf::Time time_per_frame) {
  handleExpiredExplosions();
  handleObjectOutofBound();
  handleCollision();

  spaceShip_.update(time_per_frame);

  for (auto& a : asteroids_) {
    a.update(time_per_frame);
  }

  for (auto& e : explosions_) {
    e.update(time_per_frame);
  }

  for (auto& b : bullets_) {
    b.update(time_per_frame);
  }
}

void Game::render() {
  spdlog::info("Render: {} asteroids, {} bullets, {} explosions",
               asteroids_.size(), bullets_.size(), explosions_.size());
  window_.clear();

  for (const auto& a : asteroids_) {
    window_.draw(a);
  }

  for (const auto& e : explosions_) {
    window_.draw(e);
  }

  for (const auto& b : bullets_) {
    window_.draw(b);
  }

  if (shouldRenderBounds_) {
    renderBounds();
  }

  window_.draw(spaceShip_);
  window_.draw(score_);
  window_.display();
}

void Game::handleSpaceshipFire() {
  bullets_.emplace_back(spaceShip_.Position().x, spaceShip_.Position().y,
                        spaceShip_.Angle());
}

void Game::handleCollision() {
  for (auto& a : asteroids_) {
    for (auto& b : bullets_) {
      if (a.isActive() && b.isActive() && isCollide(a.Sprite(), b.Sprite())) {
        a.setActive(false);
        b.setActive(false);
        explosions_.emplace_back(b.Bounds().left, b.Bounds().top);
      }
    }
  }

  auto pos = std::remove_if(begin(asteroids_), end(asteroids_),
                            [](auto& e) { return !e.isActive(); });
  asteroids_.erase(pos, end(asteroids_));

  auto pos2 = std::remove_if(begin(bullets_), end(bullets_),
                             [](auto& e) { return !e.isActive(); });
  bullets_.erase(pos2, end(bullets_));
}

void Game::handleObjectOutofBound() {
  const sf::FloatRect bound{0, 0, 1200, 800};

  auto pos = std::remove_if(
      begin(asteroids_), end(asteroids_),
      [&bound](auto& e) { return !e.Bounds().intersects(bound); });
  asteroids_.erase(pos, end(asteroids_));

  auto pos2 = std::remove_if(begin(bullets_), end(bullets_), [&bound](auto& e) {
    return !e.Bounds().intersects(bound);
  });
  bullets_.erase(pos2, end(bullets_));
}

void Game::handleExpiredExplosions() {
  auto pos = std::remove_if(begin(explosions_), end(explosions_),
                            [](auto& e) { return e.isDoneAnimation(); });
  explosions_.erase(pos, end(explosions_));
}

void Game::handleReset() {
  auto size = asteroids_.size();
  for (int i = 0; i + size <= 100; ++i) {
    asteroids_.emplace_back(Asteroid{});
  }
}

bool Game::isCollide(const sf::Sprite& left, const sf::Sprite& right) {
  return Collision::PixelPerfectTest(left, right);
}

void Game::renderBounds() {
  for (const auto& a : asteroids_) {
    const auto& bound = a.Bounds();
    sf::RectangleShape box{sf::Vector2f{bound.width, bound.height}};
    box.setPosition(bound.left, bound.top);
    box.setFillColor(sf::Color::Transparent);
    box.setOutlineColor(sf::Color::Red);
    box.setOutlineThickness(1.f);
    window_.draw(box);
  }

  for (const auto& b : bullets_) {
    const auto& bound = b.Bounds();
    sf::RectangleShape box{sf::Vector2f{bound.width, bound.height}};
    box.setPosition(bound.left, bound.top);
    box.setFillColor(sf::Color::Transparent);
    box.setOutlineColor(sf::Color::Blue);
    box.setOutlineThickness(1.f);
    window_.draw(box);
  }
}