#pragma once
#include "HUD/HUDComponent.hpp"

class LevelBadge : public HUDComponent {
public:
  LevelBadge(Champion *champion, const sf::Font &font);
  void draw(sf::RenderWindow &window) override;
};
