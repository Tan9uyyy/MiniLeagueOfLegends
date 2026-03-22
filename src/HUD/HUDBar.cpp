#include "HUD/HUDBar.hpp"

BarComponent::BarComponent(Champion *champion, const sf::Font &font,
                           float width, float height, sf::Color color)
    : HUDComponent(champion, font), m_width(width), m_height(height),
      m_color(color) {}

void BarComponent::draw(sf::RenderWindow &window) {
  // Fond
  sf::RectangleShape bg(sf::Vector2f(m_width, m_height));
  bg.setPosition(m_position);
  bg.setFillColor(sf::Color(50, 50, 50, 200));
  window.draw(bg);

  // Jauge
  float pct = getPercentage();
  sf::RectangleShape line(sf::Vector2f(m_width * pct, m_height));
  line.setPosition(m_position);
  line.setFillColor(m_color);
  window.draw(line);

  // Texte (si présent)
  std::string txt = getText();
  if (!txt.empty()) {
    sf::Text label(txt, m_font, 16);
    label.setPosition(m_position.x + m_width / 2.0f - 40.0f,
                      m_position.y + 2.0f);
    window.draw(label);
  }
}

// HealthBar
HealthBar::HealthBar(Champion *champion, const sf::Font &font)
    : BarComponent(champion, font, 400.0f, 25.0f, sf::Color(46, 204, 113)) {}

float HealthBar::getPercentage() const {
  return m_champion->getHealth() / m_champion->getMaxHealth();
}

std::string HealthBar::getText() const {
  return std::to_string((int)m_champion->getHealth()) + " / " +
         std::to_string((int)m_champion->getMaxHealth());
}

// ManaBar
ManaBar::ManaBar(Champion *champion, const sf::Font &font)
    : BarComponent(champion, font, 400.0f, 25.0f, sf::Color(52, 152, 219)) {}

float ManaBar::getPercentage() const {
  return m_champion->getMana() / m_champion->getMaxMana();
}

std::string ManaBar::getText() const {
  return std::to_string((int)m_champion->getMana()) + " / " +
         std::to_string((int)m_champion->getMaxMana());
}

// XPBar
XPBar::XPBar(Champion *champion, const sf::Font &font)
    : BarComponent(champion, font, 10.0f, 70.0f, sf::Color(100, 50, 200)) {}

float XPBar::getPercentage() const {
  if (m_champion->getLevel() < 18 && m_champion->getXPToNextLevel() > 0) {
    return (float)m_champion->getXP() / (float)m_champion->getXPToNextLevel();
  }
  return (m_champion->getLevel() >= 18) ? 1.0f : 0.0f;
}

void XPBar::draw(sf::RenderWindow &window) {
  // Fond
  sf::RectangleShape bg(sf::Vector2f(m_width, m_height));
  bg.setPosition(m_position);
  bg.setFillColor(sf::Color(30, 30, 30, 200));
  bg.setOutlineThickness(1.0f);
  bg.setOutlineColor(sf::Color(80, 80, 80));
  window.draw(bg);

  // Progression verticale (du bas vers le haut)
  float pct = getPercentage();
  float filledH = m_height * pct;
  sf::RectangleShape fill(sf::Vector2f(m_width, filledH));
  fill.setPosition(m_position.x, m_position.y + m_height - filledH);
  fill.setFillColor(m_color);
  window.draw(fill);
}
