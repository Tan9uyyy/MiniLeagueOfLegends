#include "Renderer.hpp"

void Renderer::renderWorld(sf::RenderWindow& window, sf::View& camera,
                           const std::vector<std::unique_ptr<Entity>>& entities) {
    window.setView(camera);
    for (const auto& entity : entities) {
        entity->draw(window);
    }
}

void Renderer::renderMinimap(sf::RenderWindow& window, sf::View& minimapView,
                             const std::vector<std::unique_ptr<Entity>>& entities) {
    window.setView(minimapView);
    sf::RectangleShape minimapBg(sf::Vector2f(10000.0f, 10000.0f));
    minimapBg.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(minimapBg);

    for (const auto& entity : entities) {
        entity->draw(window);
    }
}

void Renderer::renderUI(sf::RenderWindow& window, HUD* hud,
                         const std::vector<UIWindow*>& uiWindows) {
    // HUD d'abord
    if (hud) {
        hud->draw(window);
    }

    // Puis toutes les fenêtres ouvertes, dans l'ordre du vecteur
    for (auto* win : uiWindows) {
        if (win) win->draw(window);
    }
}
