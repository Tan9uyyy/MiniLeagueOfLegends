#pragma once
#include "HUDComponent.hpp"

class StatsPanel : public HUDComponent {
public:
    StatsPanel(Champion* champion, const sf::Font& font);
    void draw(sf::RenderWindow& window) override;
};
