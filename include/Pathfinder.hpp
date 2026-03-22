#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Map; // Forward declaration

class Pathfinder {
public:
    // Taille d'une case de la grille virtuelle (réduite pour coller de plus près aux murs)
    static constexpr float GRID_SIZE = 25.0f;

    // Calcule le chemin le plus court en évitant les obstacles
    // Retourne une liste de points de passage (waypoints) en coordonnées du monde
    static std::vector<sf::Vector2f> findPath(const Map& map, sf::Vector2f startPos, sf::Vector2f targetPos, float entityRadius);
};
