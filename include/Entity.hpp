#pragma once
#include <SFML/Graphics.hpp>

class Entity {
public:
    virtual ~Entity() = default;
    
    // Méthode de mise à jour appelée à chaque image
    // deltaTime est le temps écoulé depuis la dernière image (pour avoir des mouvements fluides)
    virtual void update(float deltaTime) = 0;
    
    // Méthode pour se dessiner à l'écran
    virtual void draw(sf::RenderWindow& window) = 0;

    // Méthode pour définir une position cible (ex: clic droit de la souris)
    virtual void setTargetPosition(sf::Vector2f /*target*/) {}

    // Obtenir la zone de collision/clic de l'entité
    virtual sf::FloatRect getBounds() const = 0;
};
