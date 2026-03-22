#pragma once
#include "HUD/HUDComponent.hpp"

class GoldDisplay : public HUDComponent {
public:
  GoldDisplay(Champion *champion, const sf::Font &font);
  void draw(sf::RenderWindow &window) override;
};

class ShopButton : public HUDComponent {
public:
  ShopButton(Champion *champion, const sf::Font &font);
  void draw(sf::RenderWindow &window) override;
  sf::FloatRect getBounds() const;
};
