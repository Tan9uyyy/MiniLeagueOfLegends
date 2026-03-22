#pragma once
#include "Entity.hpp"
#include "HUD/HUD.hpp"
#include "UIWindow.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

// Classe dédiée au rendu graphique, qui factorise toute la logique de dessin.
class Renderer {
public:
  void renderWorld(sf::RenderWindow &window, sf::View &camera,
                   const std::vector<std::unique_ptr<Entity>> &entities);

  void renderMinimap(sf::RenderWindow &window, sf::View &minimapView,
                     const std::vector<std::unique_ptr<Entity>> &entities);

  void renderUI(sf::RenderWindow &window, HUD *hud,
                const std::vector<UIWindow *> &uiWindows);
};
