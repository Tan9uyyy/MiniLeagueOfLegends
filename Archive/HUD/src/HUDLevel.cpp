#include "HUDLevel.hpp"

LevelBadge::LevelBadge(Champion* champion, const sf::Font& font)
    : HUDComponent(champion, font) {}

void LevelBadge::draw(sf::RenderWindow& window) {
    float badgeRadius = 18.0f;

    sf::CircleShape badge(badgeRadius);
    badge.setFillColor(sf::Color(30, 30, 30, 220));
    badge.setOutlineThickness(2.0f);
    badge.setOutlineColor(sf::Color(200, 180, 50));
    badge.setPosition(m_position);
    window.draw(badge);

    sf::Text levelText(std::to_string(m_champion->getLevel()), m_font, 18);
    levelText.setFillColor(sf::Color(255, 215, 0));
    
    // Center logic
    float ltX = m_position.x + badgeRadius - (m_champion->getLevel() >= 10 ? 10.0f : 5.0f);
    float ltY = m_position.y + badgeRadius - 12.0f;
    levelText.setPosition(ltX, ltY);
    window.draw(levelText);
}
