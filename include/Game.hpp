#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <cstdint>

#include "Champion.hpp"
#include "Entity.hpp"
#include "HUD/HUD.hpp"
#include "HUD/LobbyUI.hpp" // Added
#include "Map.hpp"
#include "Renderer.hpp"
#include "SettingsUI.hpp"
#include "ShopUI.hpp"
#include "UIWindow.hpp"
#include <SFML/Network.hpp>

// Added GameState enum
enum class GameState {
    MATCHMAKING,
    LOBBY,
    PLAYING
};

class Game {
public:
  Game();
  void run();

private:
  void processEvents();
  void update(float deltaTime);
  void render();
  void initWorld();
  void initViews();
  void initNetwork();
  void processNetwork();

  // Factorise l'interception des événements souris par les fenêtres UI
  bool dispatchMouseEventToWindows(const sf::Event& event);

  // Méthodes utilitaires pour le réseau
  void sendPacket(sf::Packet& packet);
  void sendMove(float x, float y);
  void sendAttack(std::uint32_t targetId);
  void sendSpell(std::uint8_t spellIndex);
  void sendBuyItem(int itemId);
  void sendSellItem(int inventoryIndex);

private:
  sf::RenderWindow m_window;
  sf::View m_camera;
  sf::View m_minimapView;
  sf::Font m_font;
  bool m_fontLoaded = false; // Changed initialization

  Map *m_gameMap;
  std::vector<std::unique_ptr<Entity>> m_entities;

  Champion *m_champion;
  // Reordered and added m_gameState and m_lobbyUI
  GameState m_gameState = GameState::MATCHMAKING; // Added
  std::unique_ptr<LobbyUI> m_lobbyUI; // Added
  std::unique_ptr<HUD> m_hud;
  std::unique_ptr<ShopUI> m_shopUI;
  std::unique_ptr<SettingsUI> m_settingsUI;

  // Vecteur de pointeurs non-owning vers toutes les fenêtres UI (ordre =
  // priorité de rendu/events)
  std::vector<UIWindow *> m_uiWindows;

  Renderer m_renderer;

  sf::UdpSocket m_socket;
  uint32_t m_localChampionId = 0;
  unsigned short m_serverPort = 0;
  std::string m_serverIp = "";
};
