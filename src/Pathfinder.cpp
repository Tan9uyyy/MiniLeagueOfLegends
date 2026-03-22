#include "Pathfinder.hpp"
#include "Map.hpp"
#include <algorithm>
#include <cmath>
#include <queue>
#include <unordered_map>

// Structure pour représenter un noeud dans la grille pour A*
struct Node {
  sf::Vector2i pos;    // Position sur la grille (index)
  float gCost;         // Coût depuis le départ
  float hCost;         // Heuristique (estimation cost to target)
  sf::Vector2i parent; // Noeud parent pour reconstruire le chemin

  float fCost() const { return gCost + hCost; }

  // Pour la file de priorité (le plus petit fCost en premier)
  bool operator>(const Node &other) const {
    if (fCost() == other.fCost()) {
      return hCost > other.hCost;
    }
    return fCost() > other.fCost();
  }
};

// Fonction de hachage personnalisée pour utiliser sf::Vector2i dans un
// unordered_map
struct Vector2iHash {
  std::size_t operator()(const sf::Vector2i &v) const {
    return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1);
  }
};
struct Vector2iEqual {
  bool operator()(const sf::Vector2i &lhs, const sf::Vector2i &rhs) const {
    return lhs.x == rhs.x && lhs.y == rhs.y;
  }
};

// Heuristique : distance euclidienne
float getDistance(const sf::Vector2i &a, const sf::Vector2i &b) {
  float dx = static_cast<float>(a.x - b.x);
  float dy = static_cast<float>(a.y - b.y);
  return std::sqrt(dx * dx + dy * dy);
}

std::vector<sf::Vector2f> Pathfinder::findPath(const Map &map,
                                               sf::Vector2f startPos,
                                               sf::Vector2f targetPos,
                                               float entityRadius) {
  std::vector<sf::Vector2f> path;

  // --- SNAP GÉOMÉTRIQUE AU MUR LE PLUS PROCHE SÉCURISE ---
  // On contraint d'abord mathématiquement la cible pour qu'elle ne soit pas
  // DANS un mur (cela permet de cliquer "derrière" un mur et de s'ancrer
  // correctement sur sa face éloignée)
  sf::Vector2f clampedTarget = targetPos;
  clampedTarget.x =
      std::clamp(clampedTarget.x, entityRadius, map.getWidth() - entityRadius);
  clampedTarget.y =
      std::clamp(clampedTarget.y, entityRadius, map.getHeight() - entityRadius);

  targetPos = map.getClosestValidPoint(clampedTarget, entityRadius);

  // --- OPTIMISATION 1: LIGNE DE MIRE DIRECTE ("EARLY EXIT") ---
  if (!map.raycast(startPos, targetPos, entityRadius)) {
    path.push_back(targetPos);
    return path;
  }

  // Convertir les positions mondiales en coordonnées de grille
  sf::Vector2i startGrid(static_cast<int>(startPos.x / GRID_SIZE),
                         static_cast<int>(startPos.y / GRID_SIZE));
  sf::Vector2i targetGrid(static_cast<int>(targetPos.x / GRID_SIZE),
                          static_cast<int>(targetPos.y / GRID_SIZE));

  // Si le centre de la case d'arrivée est dans un mur (le targetPos étant calé
  // juste au bord du mur), on doit trouver la case libre la plus proche pour
  // que A* puisse l'atteindre.
  sf::Vector2f targetCenter(targetGrid.x * GRID_SIZE + GRID_SIZE / 2.0f,
                            targetGrid.y * GRID_SIZE + GRID_SIZE / 2.0f);
  if (map.isColliding(targetCenter, entityRadius)) {
    bool found = false;
    std::queue<sf::Vector2i> searchQueue;
    std::unordered_map<sf::Vector2i, bool, Vector2iHash, Vector2iEqual>
        searched;

    searchQueue.push(targetGrid);
    searched[targetGrid] = true;

    std::vector<sf::Vector2i> searchDirs = {{0, -1},  {0, 1},  {-1, 0}, {1, 0},
                                            {-1, -1}, {1, -1}, {-1, 1}, {1, 1}};

    int searchCount = 0;
    int maxSearchLimit = 500;

    while (!searchQueue.empty() && searchCount < maxSearchLimit) {
      sf::Vector2i currentSearch = searchQueue.front();
      searchQueue.pop();
      searchCount++;

      sf::Vector2f center(currentSearch.x * GRID_SIZE + GRID_SIZE / 2.0f,
                          currentSearch.y * GRID_SIZE + GRID_SIZE / 2.0f);

      if (!map.isColliding(center, entityRadius)) {
        targetGrid = currentSearch;
        // /!\ TRÈS IMPORTANT : On ne modifie pas targetPos ici !
        // Il reste fixé parfaitement contre le mur pour le lissage final.
        found = true;
        break;
      }

      for (const auto &dir : searchDirs) {
        sf::Vector2i neighbor = currentSearch + dir;
        if (neighbor.x >= 0 && neighbor.x < map.getWidth() / GRID_SIZE &&
            neighbor.y >= 0 && neighbor.y < map.getHeight() / GRID_SIZE) {
          if (!searched[neighbor]) {
            searched[neighbor] = true;
            searchQueue.push(neighbor);
          }
        }
      }
    }

    if (!found) {
      return path; // Totalement bloqué dans un mur gigantesque
    }
  }

  if (startGrid == targetGrid) {
    path.push_back(targetPos);
    return path;
  }

  std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;
  std::unordered_map<sf::Vector2i, Node, Vector2iHash, Vector2iEqual> allNodes;
  std::unordered_map<sf::Vector2i, bool, Vector2iHash, Vector2iEqual> closedSet;

  Node startNode = {startGrid, 0.0f, getDistance(startGrid, targetGrid),
                    startGrid};
  openSet.push(startNode);
  allNodes[startGrid] = startNode;

  std::vector<sf::Vector2i> directions = {{0, -1},  {0, 1},  {-1, 0}, {1, 0},
                                          {-1, -1}, {1, -1}, {-1, 1}, {1, 1}};

  // On limite drastiquement les itérations pour éviter le lag si complexe (1000
  // suffisent avec la ligne de mire)
  int maxIterations = 1000;
  int iterations = 0;

  std::vector<sf::Vector2f> rawPath; // Chemin brut d'A*

  while (!openSet.empty() && iterations < maxIterations) {
    iterations++;
    Node current = openSet.top();
    openSet.pop();

    if (closedSet[current.pos])
      continue;
    closedSet[current.pos] = true;

    if (current.pos == targetGrid) {
      sf::Vector2i currPos = current.pos;
      while (currPos != startGrid) {
        rawPath.push_back(
            sf::Vector2f(currPos.x * GRID_SIZE + GRID_SIZE / 2.0f,
                         currPos.y * GRID_SIZE + GRID_SIZE / 2.0f));
        currPos = allNodes[currPos].parent;
      }
      rawPath.push_back(
          sf::Vector2f(startGrid.x * GRID_SIZE + GRID_SIZE / 2.0f,
                       startGrid.y * GRID_SIZE + GRID_SIZE / 2.0f));
      std::reverse(rawPath.begin(), rawPath.end());

      if (!rawPath.empty()) {
        rawPath.back() = targetPos; // Remplacer dernier point
        // Remplacer premier point
        rawPath.front() = startPos;
      }
      break;
    }

    for (const auto &dir : directions) {
      sf::Vector2i neighborPos = current.pos + dir;
      if (closedSet[neighborPos])
        continue;

      sf::Vector2f worldCenter(neighborPos.x * GRID_SIZE + GRID_SIZE / 2.0f,
                               neighborPos.y * GRID_SIZE + GRID_SIZE / 2.0f);

      if (map.isColliding(worldCenter, entityRadius)) {
        continue;
      }

      // Pénaliser les diagonales en rasant les murs (corner cutting)
      float moveCost =
          (dir.x != 0 && dir.y != 0) ? 1.414f * GRID_SIZE : GRID_SIZE;

      // Éviter qu'on coupe les angles à travers les murs
      if (dir.x != 0 && dir.y != 0) {
        sf::Vector2f w1((current.pos.x + dir.x) * GRID_SIZE + GRID_SIZE / 2.0f,
                        current.pos.y * GRID_SIZE + GRID_SIZE / 2.0f);
        sf::Vector2f w2(current.pos.x * GRID_SIZE + GRID_SIZE / 2.0f,
                        (current.pos.y + dir.y) * GRID_SIZE + GRID_SIZE / 2.0f);
        if (map.isColliding(w1, entityRadius) ||
            map.isColliding(w2, entityRadius)) {
          continue; // Pas le droit de couper un coin serré
        }
      }

      float tentativeGCost = current.gCost + moveCost;

      if (allNodes.find(neighborPos) == allNodes.end() ||
          tentativeGCost < allNodes[neighborPos].gCost) {
        Node neighborNode;
        neighborNode.pos = neighborPos;
        neighborNode.gCost = tentativeGCost;
        neighborNode.hCost = getDistance(neighborPos, targetGrid) * GRID_SIZE;
        neighborNode.parent = current.pos;

        allNodes[neighborPos] = neighborNode;
        openSet.push(neighborNode);
      }
    }
  }

  // --- OPTIMISATION 2: STRING PULLING (LISSAGE DU CHEMIN) ---
  // On transforme l'escalier brut en belles diagonales rectilignes
  if (rawPath.size() <= 2) {
    return rawPath;
  }

  path.push_back(rawPath.front());
  size_t currentIndex = 0;

  while (currentIndex < rawPath.size() - 1) {
    size_t furthestVisibleIndex = currentIndex + 1;

    // On cherche le point le plus lointain qu'on peut atteindre en ligne droite
    for (size_t nextIndex = currentIndex + 2; nextIndex < rawPath.size();
         ++nextIndex) {
      // Si la ligne de vue n'est PAS obstruée, on peut sauter jusque là
      if (!map.raycast(rawPath[currentIndex], rawPath[nextIndex],
                       entityRadius)) {
        furthestVisibleIndex = nextIndex;
      } else {
        // Dès qu'on trouve un mur, le point précédent était le dernier visible
        break;
      }
    }

    // On ajoute le point visible le plus lointain
    path.push_back(rawPath[furthestVisibleIndex]);
    currentIndex = furthestVisibleIndex;
  }

  return path;
}
