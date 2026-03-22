#pragma once
#include "HUDComponent.hpp"

class SpellsDisplay : public HUDComponent {
public:
    SpellsDisplay(Champion* champion, const sf::Font& font);
    void draw(sf::RenderWindow& window) override;
};
