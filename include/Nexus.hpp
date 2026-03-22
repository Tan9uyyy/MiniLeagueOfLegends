#pragma once
#include "CombatEntity.hpp"

class Nexus : public CombatEntity {
public:
    Nexus(sf::Vector2f position, Team team);
    
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;

    sf::FloatRect getBounds() const override;

private:
    sf::RectangleShape m_shape;
};
