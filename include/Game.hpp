#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

#include "Champion.hpp"
#include "Entity.hpp"
#include "HUD/HUD.hpp"
#include "Map.hpp"
#include "Renderer.hpp"
#include "SettingsUI.hpp"
#include "ShopUI.hpp"
#include "UIWindow.hpp"

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

  // Factorise l'interception des événements souris par les fenêtres UI
  bool dispatchMouseEventToWindows(const sf::Event &event);

private:
  sf::RenderWindow m_window;
  sf::View m_camera;
  sf::View m_minimapView;
  sf::Font m_font;
  bool m_fontLoaded;

  Map *m_gameMap;
  std::vector<std::unique_ptr<Entity>> m_entities;

  Champion *m_champion;
  std::unique_ptr<HUD> m_hud;
  std::unique_ptr<ShopUI> m_shopUI;
  std::unique_ptr<SettingsUI> m_settingsUI;

  // Vecteur de pointeurs non-owning vers toutes les fenêtres UI (ordre =
  // priorité de rendu/events)
  std::vector<UIWindow *> m_uiWindows;

  Renderer m_renderer;
};
