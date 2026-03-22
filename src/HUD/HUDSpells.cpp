#include "HUD/HUDSpells.hpp"

SpellsDisplay::SpellsDisplay(Champion *champion, const sf::Font &font)
    : HUDComponent(champion, font) {}

void SpellsDisplay::draw(sf::RenderWindow &window) {
  float spellSize = 50.0f;
  float spellGap = 8.0f;
  const auto &spells = m_champion->getSpells();
  bool hasSkillPoints = m_champion->getSkillPoints() > 0;

  for (int i = 0; i < 4; ++i) {
    const auto &spell = spells[i];
    float sx = m_position.x + i * (spellSize + spellGap);
    float sy = m_position.y;

    // Fond du sort
    sf::RectangleShape spellBg(sf::Vector2f(spellSize, spellSize));
    spellBg.setPosition(sx, sy);

    if (spell.level == 0) {
      spellBg.setFillColor(sf::Color(30, 30, 30, 200));
    } else if (spell.currentCooldown > 0.0f) {
      spellBg.setFillColor(sf::Color(60, 30, 30, 200));
    } else {
      spellBg.setFillColor(sf::Color(40, 60, 80, 200));
    }

    spellBg.setOutlineThickness(1.5f);
    spellBg.setOutlineColor(spell.level > 0 ? sf::Color(200, 180, 50)
                                            : sf::Color(80, 80, 80));
    window.draw(spellBg);

    // Lettre du sort
    sf::Text keyText(spell.key, m_font, 20);
    keyText.setFillColor(spell.level > 0 ? sf::Color::White
                                         : sf::Color(100, 100, 100));
    keyText.setPosition(sx + 15.0f, sy + 5.0f);
    window.draw(keyText);

    // Niveau du sort
    if (spell.level > 0) {
      sf::Text lvlText("Nv." + std::to_string(spell.level), m_font, 10);
      lvlText.setFillColor(sf::Color(200, 200, 200));
      lvlText.setPosition(sx + 5.0f, sy + 35.0f);
      window.draw(lvlText);
    }

    // Cooldown restant
    if (spell.level > 0 && spell.currentCooldown > 0.0f) {
      sf::Text cdText(std::to_string((int)spell.currentCooldown + 1), m_font,
                      18);
      cdText.setFillColor(sf::Color::White);
      cdText.setPosition(sx + 18.0f, sy + 15.0f);
      window.draw(cdText);
    }

    // Indicateur "+" (Amélioration)
    if (hasSkillPoints && spell.canLevelUp()) {
      sf::CircleShape plusBg(8.0f);
      plusBg.setFillColor(sf::Color(50, 200, 50));
      plusBg.setPosition(sx + spellSize - 16.0f, sy - 4.0f);
      window.draw(plusBg);

      sf::Text plusText("+", m_font, 12);
      plusText.setFillColor(sf::Color::White);
      plusText.setPosition(sx + spellSize - 13.0f, sy - 6.0f);
      window.draw(plusText);
    }
  }
}
