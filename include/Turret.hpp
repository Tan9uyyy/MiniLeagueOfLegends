#pragma once
#include "CombatEntity.hpp"

class Turret : public CombatEntity {
public:
    Turret(sf::Vector2f position, Team team);
    
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;

    sf::FloatRect getBounds() const override;

private:
    sf::CircleShape m_baseShape;
    sf::CircleShape m_rangeShape;
    float m_attackRange;
};
