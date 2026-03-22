#include "ClickIndicator.hpp"

ClickIndicator::ClickIndicator() : m_lifeTime(0.0f), m_maxLifeTime(0.4f), m_isActive(false) {
    // On initialise la forme (un cercle vide avec une bordure)
    m_shape.setFillColor(sf::Color::Transparent);
    m_shape.setOutlineThickness(2.0f);
}

void ClickIndicator::setTargetPosition(sf::Vector2f target) {
    // Réaction au clic : on se place là où le joueur a cliqué et on active l'animation
    m_shape.setPosition(target);
    m_lifeTime = 0.0f;
    m_isActive = true;
}

void ClickIndicator::update(float deltaTime) {
    if (m_isActive) {
        m_lifeTime += deltaTime;
        
        // Si l'animation est terminée, on la désactive
        if (m_lifeTime >= m_maxLifeTime) {
            m_isActive = false;
        } else {
            // Calcul de la progression de l'animation entre 0.0 (début) et 1.0 (fin)
            float ratio = m_lifeTime / m_maxLifeTime;
            
            // Animation : le rayon rétrécit de 25 pixels jusqu'à 5 pixels au centre
            float radius = 25.0f - (20.0f * ratio);
            m_shape.setRadius(radius);
            m_shape.setOrigin(radius, radius); // On recentre l'origine pour que le rétrécissement se fasse par le milieu
            
            // Animation : disparition progressive (fondu)
            sf::Uint8 alpha = static_cast<sf::Uint8>(255.0f * (1.0f - ratio));
            sf::Color color = sf::Color::Green; 
            color.a = alpha; // Modification du canal de transparence (Alpha)
            m_shape.setOutlineColor(color);
        }
    }
}

void ClickIndicator::draw(sf::RenderWindow& window) {
    // On ne dessine l'indicateur que s'il est actif
    if (m_isActive) {
        window.draw(m_shape);
    }
}
