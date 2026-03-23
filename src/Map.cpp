#include "Map.hpp"
#include <cmath>
#include <algorithm>

Map::Map(float width, float height) : m_width(width), m_height(height) {
    // Le fond de la carte (vert très foncé pour simuler l'herbe)
    m_ground.setSize(sf::Vector2f(width, height));
    m_ground.setFillColor(sf::Color(34, 139, 34, 100)); // Forest Green avec de la transparence
    m_ground.setPosition({0.0f, 0.0f});

    // On crée une grille au sol pour mieux se repérer visuellement lors du déplacement
    float gridSize = 100.0f; // Une ligne tous les 100 pixels

    // Lignes verticales
    for (float x = 0; x <= width; x += gridSize) {
        sf::RectangleShape line(sf::Vector2f(2.0f, height));
        line.setPosition({x, 0.0f});
        line.setFillColor(sf::Color(255, 255, 255, 30)); // Blanc très transparent
        m_gridLines.push_back(line);
    }

    // Lignes horizontales
    for (float y = 0; y <= height; y += gridSize) {
        sf::RectangleShape line(sf::Vector2f(width, 2.0f));
        line.setPosition({0.0f, y});
        line.setFillColor(sf::Color(255, 255, 255, 30));
        m_gridLines.push_back(line);
    }

    // --- Ajout d'obstacles pour tester les collisions ---
    // Mur horizontal au-dessus du centre
    sf::RectangleShape wall1(sf::Vector2f(400.0f, 50.0f));
    wall1.setPosition({4800.0f, 4700.0f});
    wall1.setFillColor(sf::Color(100, 100, 100)); // Gris (Pierre)
    m_obstacles.push_back(wall1);

    // Mur vertical à droite du centre
    sf::RectangleShape wall2(sf::Vector2f(50.0f, 300.0f));
    wall2.setPosition({5300.0f, 4800.0f});
    wall2.setFillColor(sf::Color(100, 100, 100));
    m_obstacles.push_back(wall2);

    // Un "T" formé de deux murs en bas à gauche
    sf::RectangleShape wall3(sf::Vector2f(300.0f, 50.0f));
    wall3.setPosition({4500.0f, 5300.0f});
    wall3.setFillColor(sf::Color(139, 69, 19)); // Marron (Bois/Arbre)
    m_obstacles.push_back(wall3);

    sf::RectangleShape wall4(sf::Vector2f(50.0f, 200.0f));
    wall4.setPosition({4625.0f, 5100.0f});
    wall4.setFillColor(sf::Color(139, 69, 19));
    m_obstacles.push_back(wall4);

    // --- Configuration des zones d'apparition ---
    float spawnRadius = 400.0f;
    m_alliedSpawn.setRadius(spawnRadius);
    m_alliedSpawn.setOrigin({spawnRadius, spawnRadius});
    m_alliedSpawn.setPosition({1300.0f, 8800.0f}); // Centre = position de départ du champion
    m_alliedSpawn.setFillColor(sf::Color(0, 0, 255, 40)); // Bleu transparent
    m_alliedSpawn.setOutlineThickness(2.0f);
    m_alliedSpawn.setOutlineColor(sf::Color(100, 100, 255, 150));

    m_enemySpawn.setRadius(spawnRadius);
    m_enemySpawn.setOrigin({spawnRadius, spawnRadius});
    m_enemySpawn.setPosition({2600.0f, 7900.0f}); // Derrière le nexus ennemi
    m_enemySpawn.setFillColor(sf::Color(255, 0, 0, 40)); // Rouge transparent
    m_enemySpawn.setOutlineThickness(2.0f);
    m_enemySpawn.setOutlineColor(sf::Color(255, 100, 100, 150));
}

void Map::update(float /*deltaTime*/) {
    // Le décor ne bouge pas
}

void Map::draw(sf::RenderWindow& window) {
    window.draw(m_ground);

    for (const auto& line : m_gridLines) {
        window.draw(line);
    }

    // On dessine aussi nos obstacles
    for (const auto& obs : m_obstacles) {
        window.draw(obs);
    }

    // On dessine les zones d'apparition
    window.draw(m_alliedSpawn);
    window.draw(m_enemySpawn);
}

void Map::addObstacle(const sf::FloatRect& bounds) {
    sf::RectangleShape newObs(bounds.size);
    newObs.setPosition(bounds.position);
    // On le met transparent car le bâtiment dessine déjà son propre sprite/shape par dessus
    newObs.setFillColor(sf::Color::Transparent); 
    m_obstacles.push_back(newObs);
}

sf::Vector2f Map::getSpawnPosition(Team team) const {
    if (team == Team::ALLIED) return m_alliedSpawn.getPosition();
    return m_enemySpawn.getPosition();
}

bool Map::isInSpawnArea(const sf::Vector2f& position, Team team) const {
    const sf::CircleShape& spawnZone = (team == Team::ALLIED) ? m_alliedSpawn : m_enemySpawn;
    sf::Vector2f center = spawnZone.getPosition();
    float dx = position.x - center.x;
    float dy = position.y - center.y;
    float distanceSquared = dx * dx + dy * dy;
    float radius = spawnZone.getRadius();
    return distanceSquared <= (radius * radius);
}

bool Map::isColliding(const sf::Vector2f& position, float radius) const {
    // 1. Détection avec les bords de la carte
    if (position.x - radius < 0.0f || position.x + radius > m_width ||
        position.y - radius < 0.0f || position.y + radius > m_height) {
        return true;
    }

    // 2. Détection avec les obstacles (Cercle contre Rectangle)
    for (const auto& obs : m_obstacles) {
        sf::FloatRect rect = obs.getGlobalBounds();

        // On cherche le point le plus proche du cercle sur le rectangle
        float testX = position.x;
        float testY = position.y;

        if (position.x < rect.position.x) testX = rect.position.x;
        else if (position.x > rect.position.x + rect.size.x) testX = rect.position.x + rect.size.x;

        if (position.y < rect.position.y) testY = rect.position.y;
        else if (position.y > rect.position.y + rect.size.y) testY = rect.position.y + rect.size.y;

        // On calcule la distance entre le centre du cercle et ce point le plus proche
        float distX = position.x - testX;
        float distY = position.y - testY;
        float distance = std::sqrt(distX * distX + distY * distY);

        if (distance <= radius) {
            return true; // Collision !
        }
    }

    return false;
}

// Algorithme géométrique pour "pousser" un point hors de tous les rectangles
// C'est beaucoup plus précis et proche des bords que le BFS sur grille
sf::Vector2f Map::getClosestValidPoint(sf::Vector2f position, float radius) const {
    // 1. On contraint d'abord aux bordures du monde
    position.x = std::clamp(position.x, radius, m_width - radius);
    position.y = std::clamp(position.y, radius, m_height - radius);

    // 2. On fait quelques itérations pour résoudre les collisions multiples (coins)
    for (int i = 0; i < 5; ++i) {
        bool hitSomething = false;
        
        for (const auto& obs : m_obstacles) {
            sf::FloatRect rect = obs.getGlobalBounds();

            float testX = position.x;
            float testY = position.y;

            if (position.x < rect.position.x) testX = rect.position.x;
            else if (position.x > rect.position.x + rect.size.x) testX = rect.position.x + rect.size.x;

            if (position.y < rect.position.y) testY = rect.position.y;
            else if (position.y > rect.position.y + rect.size.y) testY = rect.position.y + rect.size.y;

            float distX = position.x - testX;
            float distY = position.y - testY;
            float distance = std::sqrt(distX * distX + distY * distY);

            // S'il y a collision, on repousse sur le vecteur de distance
            if (distance < radius) {
                hitSomething = true;
                if (distance == 0.0f) {
                    // Le point est PILE au centre du rectangle, il faut le rejeter vers le bord le plus proche
                    float distLeft = position.x - rect.position.x;
                    float distRight = (rect.position.x + rect.size.x) - position.x;
                    float distTop = position.y - rect.position.y;
                    float distBottom = (rect.position.y + rect.size.y) - position.y;
                    
                    float minDist = std::min({distLeft, distRight, distTop, distBottom});
                    
                    // On pousse le centre de notre entité hors du rectangle (+ son rayon)
                    // Ajout d'un petit epsilon (0.1f) pour s'assurer qu'on n'est plus tout à fait en collision
                    if (minDist == distLeft) position.x = rect.position.x - radius - 0.1f;
                    else if (minDist == distRight) position.x = rect.position.x + rect.size.x + radius + 0.1f;
                    else if (minDist == distTop) position.y = rect.position.y - radius - 0.1f;
                    else position.y = rect.position.y + rect.size.y + radius + 0.1f;
                } else {
                    // Le point n'est pas au centre, on le repousse simplement de "l'Overlap"
                    float overlap = radius - distance + 0.1f;
                    position.x += (distX / distance) * overlap;
                    position.y += (distY / distance) * overlap;
                }
            }
        }
        
        // Si aucune collision corrigée dans cette passe, c'est bon
        if (!hitSomething) break;
    }

    return position;
}

// Trouve le point d'impact exact sur la bordure de l'obstacle le plus proche sur la ligne de vue
sf::Vector2f Map::getValidPointOnLine(const sf::Vector2f& start, const sf::Vector2f& end, float radius) const {
    sf::Vector2f direction = end - start;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    
    if (distance == 0.0f) return start;
    
    direction.x /= distance;
    direction.y /= distance;
    
    float stepSize = radius * 0.5f;
    if (stepSize < 2.0f) stepSize = 2.0f;
    
    float currentDist = 0.0f;
    sf::Vector2f lastValid = start;
    
    while (currentDist <= distance) {
        sf::Vector2f testPos = start + (direction * currentDist);
        if (isColliding(testPos, radius)) {
            // On a tapé le mur ! On recule légèrement pour être sûr de ne pas être en collision (epsilon)
            return lastValid;
        }
        lastValid = testPos;
        currentDist += stepSize;
    }
    
    return end; // Aucun obstacle trouvé, on renvoie la fin
}

// Raycast basique en échantillonnant des points sur la ligne
// Cette méthode avance du 'start' au 'end' et vérifie des "cercles" successifs
bool Map::raycast(const sf::Vector2f& start, const sf::Vector2f& end, float radius) const {
    sf::Vector2f direction = end - start;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    
    // Si c'est trop court, on vérifie juste de base
    if (distance <= radius) {
        return isColliding(end, radius);
    }
    
    direction.x /= distance;
    direction.y /= distance;
    
    // On avance par "pas" légèrement inférieurs au rayon pour ne rien rater (ex: rayon/2)
    float stepSize = radius * 0.5f; 
    if (stepSize < 5.0f) stepSize = 5.0f; // Pas trop petit pour perfs
    
    float currentDist = 0.0f;
    while (currentDist < distance) {
        sf::Vector2f testPos = start + (direction * currentDist);
        if (isColliding(testPos, radius)) {
            return true; // Un obstacle bloque la ligne de vue
        }
        currentDist += stepSize;
    }
    
    // On vérifie le point d'arrivée exact
    return isColliding(end, radius);
}
