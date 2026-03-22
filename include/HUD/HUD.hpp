#pragma once
#include "HUD/HUDBar.hpp"
#include "HUD/HUDGold.hpp"
#include "HUD/HUDLevel.hpp"
#include "HUD/HUDSpells.hpp"
#include "HUD/HUDStats.hpp"
#include <memory>

class HUD {
public:
  HUD(Champion *champion, const sf::Font &font);
  void draw(sf::RenderWindow &window);
  bool isShopButtonClicked(const sf::Vector2f &mousePos) const;

private:
  Champion *m_champion;
  const sf::Font &m_font;

  std::unique_ptr<HealthBar> m_hpBar;
  std::unique_ptr<ManaBar> m_mpBar;
  std::unique_ptr<XPBar> m_xpBar;
  std::unique_ptr<StatsPanel> m_statsPanel;
  std::unique_ptr<SpellsDisplay> m_spellsDisplay;
  std::unique_ptr<LevelBadge> m_levelBadge;
  std::unique_ptr<GoldDisplay> m_goldDisplay;
  std::unique_ptr<ShopButton> m_shopButton;
};
