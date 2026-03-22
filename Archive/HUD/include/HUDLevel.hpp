#pragma once
#include "HUDComponent.hpp"

class LevelBadge : public HUDComponent {
public:
    LevelBadge(Champion* champion, const sf::Font& font);
    void draw(sf::RenderWindow& window) override;
};
