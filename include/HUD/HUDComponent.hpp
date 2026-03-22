#pragma once
#include <SFML/Graphics.hpp>
#include "Champion.hpp"

class HUDComponent {
public:
    virtual ~HUDComponent() = default;
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual void setPosition(float x, float y) { m_position = {x, y}; }

protected:
    sf::Vector2f m_position;
    Champion* m_champion;
    const sf::Font& m_font;

    HUDComponent(Champion* champion, const sf::Font& font)
        : m_champion(champion), m_font(font) {}
};
