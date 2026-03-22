#include "HUD/HUDStats.hpp"
#include <iomanip>
#include <sstream>

StatsPanel::StatsPanel(Champion *champion, const sf::Font &font)
    : HUDComponent(champion, font) {}

void StatsPanel::draw(sf::RenderWindow &window) {
  // Arrière-plan
  sf::RectangleShape bg(sf::Vector2f(160.0f, 85.0f));
  bg.setPosition(m_position);
  bg.setFillColor(sf::Color(30, 30, 30, 200));
  bg.setOutlineThickness(1.0f);
  bg.setOutlineColor(sf::Color(80, 80, 80));
  window.draw(bg);

  sf::Text title(m_font, "STATS", 12);
  title.setPosition({m_position.x + 5.0f, m_position.y + 3.0f});
  title.setFillColor(sf::Color(200, 200, 200));
  window.draw(title);

  sf::Text adText(m_font, "AD : " + std::to_string((int)m_champion->getAttackDamage()),
                  14);
  adText.setPosition({m_position.x + 5.0f, m_position.y + 23.0f});
  window.draw(adText);

  std::ostringstream ss;
  ss << std::fixed << std::setprecision(2) << m_champion->getAttackSpeed();
  sf::Text asText(m_font, "AS : " + ss.str(), 14);
  asText.setPosition({m_position.x + 5.0f, m_position.y + 43.0f});
  window.draw(asText);

  sf::Text rangeText(m_font, "Portee : " +
                         std::to_string((int)m_champion->getAttackRange()),
                     14);
  rangeText.setPosition({m_position.x + 5.0f, m_position.y + 63.0f});
  window.draw(rangeText);
}
