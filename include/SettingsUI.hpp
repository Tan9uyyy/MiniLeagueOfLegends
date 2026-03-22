#pragma once
#include "UIWindow.hpp"

enum class SettingsCategory {
    VIDEO,
    AUDIO,
    GAME
};

class SettingsUI : public UIWindow {
public:
    SettingsUI(const sf::Font& font);

protected:
    UIAction onHandleEvent(const sf::Event& event, const sf::Vector2f& mousePos) override;
    void onDraw(sf::RenderWindow& window, const sf::Vector2f& basePos) override;

private:
    SettingsCategory m_selectedCategory;
};
