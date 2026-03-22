#include "HUDGold.hpp"

// GoldDisplay
GoldDisplay::GoldDisplay(Champion* champion, const sf::Font& font)
    : HUDComponent(champion, font) {}

void GoldDisplay::draw(sf::RenderWindow& window) {
    sf::Text goldText("GOLDS : " + std::to_string((int)m_champion->getGold()), m_font, 20);
    goldText.setFillColor(sf::Color::Yellow);
    goldText.setPosition(m_position);
    window.draw(goldText);
}

// ShopButton
ShopButton::ShopButton(Champion* champion, const sf::Font& font)
    : HUDComponent(champion, font) {}

void ShopButton::draw(sf::RenderWindow& window) {
    sf::RectangleShape shopBtn(sf::Vector2f(130.0f, 30.0f));
    shopBtn.setPosition(m_position);
    shopBtn.setFillColor(sf::Color(100, 100, 100, 200));
    shopBtn.setOutlineThickness(2.0f);
    shopBtn.setOutlineColor(sf::Color(200, 150, 50));
    window.draw(shopBtn);

    sf::Text shopText("BOUTIQUE [P]", m_font, 14);
    shopText.setPosition(m_position.x + 10.0f, m_position.y + 6.0f);
    window.draw(shopText);
}

sf::FloatRect ShopButton::getBounds() const {
    return sf::FloatRect(m_position.x, m_position.y, 130.0f, 30.0f);
}
