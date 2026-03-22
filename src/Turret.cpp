#include "Turret.hpp"

Turret::Turret(sf::Vector2f position, Team team)
    : CombatEntity(team, 2500.0f), m_attackRange(600.0f) { // Portée d'attaque typique
    
    // Base de la tourelle
    m_baseShape.setRadius(50.0f);
    
    // Cercle de portée visuelle (transparent)
    m_rangeShape.setRadius(m_attackRange);
    
    if (team == Team::ALLIED) {
        m_baseShape.setFillColor(sf::Color(0, 0, 180));
        m_rangeShape.setFillColor(sf::Color(0, 0, 255, 30)); // Tracer très transparent
        m_rangeShape.setOutlineColor(sf::Color(0, 0, 255, 100));
    } else {
        m_baseShape.setFillColor(sf::Color(180, 0, 0));
        m_rangeShape.setFillColor(sf::Color(255, 0, 0, 30));
        m_rangeShape.setOutlineColor(sf::Color(255, 0, 0, 100));
    }
    
    m_rangeShape.setOutlineThickness(2.0f);
    
    m_baseShape.setOrigin({50.0f, 50.0f});
    m_rangeShape.setOrigin({m_attackRange, m_attackRange});
    
    m_baseShape.setPosition(position);
    m_rangeShape.setPosition(position);
}

void Turret::update(float /*deltaTime*/) {
    // Plus tard : Logique de détection des ennemis à portée et de tir (Projectiles)
}

void Turret::draw(sf::RenderWindow& window) {
    if (!isDead()) {
        window.draw(m_rangeShape); // Dessiner la portée sous la tourelle
        window.draw(m_baseShape);
        drawHealthBar(window, m_baseShape.getPosition(), 60.0f, 6.0f);
    }
}

sf::FloatRect Turret::getBounds() const {
    return m_baseShape.getGlobalBounds();
}
