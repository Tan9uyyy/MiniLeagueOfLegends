#pragma once
#include <SFML/System/Vector2.hpp>
#include <string>

namespace Config {
namespace Network {
inline constexpr const char *MASTER_SERVER_IP = "127.0.0.1";
inline constexpr int MASTER_SERVER_PORT = 50000;
inline constexpr int PLAYERS_PER_MATCH = 2; // For testing

inline constexpr const char *SERVER_IP = "127.0.0.1";
inline constexpr int SERVER_PORT = 54321;
inline constexpr float TICK_RATE = 1.0f / 30.0f;
} // namespace Network

namespace Game {
inline constexpr float MAP_WIDTH = 10000.0f;
inline constexpr float MAP_HEIGHT = 10000.0f;
inline constexpr int FPS_LIMIT = 60;
inline const std::string WINDOW_TITLE = "Mini League of Legends";
inline constexpr float MINIMAP_SIZE = 350.0f;
} // namespace Game

namespace Champion {
inline constexpr float SPEED = 320.0f;
inline constexpr float RADIUS = 20.0f;
inline constexpr float RECALL_DURATION = 8.0f;
inline constexpr int MAX_LEVEL = 18;
} // namespace Champion

namespace Map {
inline constexpr float GRID_SIZE = 100.0f;
inline constexpr float SPAWN_RADIUS = 400.0f;

// Positions
inline const sf::Vector2f ALLIED_NEXUS_POS{1000.0f, 9000.0f};
inline const sf::Vector2f ALLIED_TURRET_1_POS{1200.0f, 8500.0f};
inline const sf::Vector2f ALLIED_TURRET_2_POS{1500.0f, 8800.0f};
inline const sf::Vector2f ALLIED_SPAWN_POS{1300.0f, 8800.0f};

inline const sf::Vector2f ENEMY_NEXUS_POS{2500.0f, 8000.0f};
inline const sf::Vector2f ENEMY_TURRET_1_POS{2200.0f, 8500.0f};
inline const sf::Vector2f ENEMY_TURRET_2_POS{2500.0f, 8300.0f};
inline const sf::Vector2f ENEMY_SPAWN_POS{2600.0f, 7900.0f};
} // namespace Map
} // namespace Config
