#pragma once
#include "Entity.hpp"

// Classe gérant l'animation visuelle au sol lors d'un clic
class ClickIndicator : public Entity {
public:
    ClickIndicator();

    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    
    // On utilise la même méthode que le champion pour réagir au clic
    void setTargetPosition(sf::Vector2f target) override;

    sf::FloatRect getBounds() const override { return sf::FloatRect(); }

private:
    sf::CircleShape m_shape;
    float m_lifeTime;      // Temps écoulé depuis le clic
    float m_maxLifeTime;   // Durée totale de l'animation
    bool m_isActive;       // Vrai si on doit afficher l'animation
};
