#include "Nexus.hpp"

Nexus::Nexus(sf::Vector2f position, Team team)
    : CombatEntity(team, 5000.0f) { // Un Nexus a beaucoup de points de vie
    
    // Taille massive pour le Nexus
    m_shape.setSize({200.0f, 200.0f});
    
    if (team == Team::ALLIED) {
        m_shape.setFillColor(sf::Color(0, 0, 255, 200)); // Bleu translucide
    } else {
        m_shape.setFillColor(sf::Color(255, 0, 0, 200)); // Rouge translucide
    }
    
    m_shape.setOrigin({100.0f, 100.0f});
    m_shape.setPosition(position);
}

void Nexus::update(float /*deltaTime*/) {
    // Si la santé tombe à 0, le jeu est terminé (logique à faire plus tard)
}

void Nexus::draw(sf::RenderWindow& window) {
    // Ne pas dessiner s'il est mort
    if (!isDead()) {
        window.draw(m_shape);
        drawHealthBar(window, m_shape.getPosition(), 100.0f, 8.0f);
    }
}

sf::FloatRect Nexus::getBounds() const {
    return m_shape.getGlobalBounds();
}
