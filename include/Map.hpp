#pragma once
#include "Entity.hpp"
#include <vector>

class Map : public Entity {
public:
    Map(float width, float height);

    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;

    // Vérifie si une position donnée (avec un rayon) entre en collision avec le décor
    bool isColliding(const sf::Vector2f& position, float radius) const;

    // Renvoie la position la plus proche qui n'est pas en collision avec un obstacle
    sf::Vector2f getClosestValidPoint(sf::Vector2f position, float radius) const;

    // Renvoie le dernier point valide sur la ligne entre start et end avant une collision
    sf::Vector2f getValidPointOnLine(const sf::Vector2f& start, const sf::Vector2f& end, float radius) const;

    // Vérifie si une ligne droite entre start et end (avec une épaisseur/rayon) croise un obstacle
    bool raycast(const sf::Vector2f& start, const sf::Vector2f& end, float radius) const;

    sf::FloatRect getBounds() const override { return sf::FloatRect({0.0f, 0.0f}, {m_width, m_height}); }
    float getWidth() const { return m_width; }
    float getHeight() const { return m_height; }

    // Ajoute un obstacle rectangulaire existant (pour les Bâtiments)
    void addObstacle(const sf::FloatRect& bounds);

private:
    float m_width;
    float m_height;
    sf::RectangleShape m_ground;
    std::vector<sf::RectangleShape> m_gridLines;
    std::vector<sf::RectangleShape> m_obstacles; // Murs / Arbres
};
