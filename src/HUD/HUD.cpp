#include "HUD/HUD.hpp"

HUD::HUD(Champion *champion, const sf::Font &font)
    : m_champion(champion), m_font(font) {

  m_hpBar = std::make_unique<HealthBar>(champion, font);
  m_mpBar = std::make_unique<ManaBar>(champion, font);
  m_xpBar = std::make_unique<XPBar>(champion, font);
  m_statsPanel = std::make_unique<StatsPanel>(champion, font);
  m_spellsDisplay = std::make_unique<SpellsDisplay>(champion, font);
  m_levelBadge = std::make_unique<LevelBadge>(champion, font);
  m_goldDisplay = std::make_unique<GoldDisplay>(champion, font);
  m_shopButton = std::make_unique<ShopButton>(champion, font);
}

void HUD::draw(sf::RenderWindow &window) {
  if (!m_champion)
    return;

  // On met la vue UI
  window.setView(window.getDefaultView());

  float screenW = window.getDefaultView().getSize().x;
  float screenH = window.getDefaultView().getSize().y;

  if (m_champion->isDead()) {
    float timer = m_champion->getRespawnTimer();
    sf::Text deathText(
        m_font,
        "REAPPARITION DANS : " + std::to_string((int)std::ceil(timer)) + "s",
        30);
    deathText.setFillColor(sf::Color::Red);
    sf::FloatRect bounds = deathText.getLocalBounds();
    deathText.setPosition(
        {screenW / 2.0f - bounds.size.x / 2.0f, screenH / 4.0f});

    sf::RectangleShape bg(
        sf::Vector2f(bounds.size.x + 40.0f, bounds.size.y + 20.0f));
    bg.setFillColor(sf::Color(0, 0, 0, 200));
    bg.setPosition(
        {deathText.getPosition().x - 20.0f, deathText.getPosition().y - 10.0f});

    window.draw(bg);
    window.draw(deathText);
    // We continue to draw the rest of the HUD even if dead
  }

  // --- CALCUL DES POSITIONS (Centralisé ici pour la cohérence) ---
  float barWidth = 400.0f;
  float barX = screenW / 2.0f - barWidth / 2.0f;
  float barY = screenH - 60.0f;
  float leftStartX = barX - 180.0f;
  float rightStartX = barX + barWidth + 30.0f;

  // Positionnement des composants
  m_hpBar->setPosition(barX, barY - 30.0f);
  m_mpBar->setPosition(barX, barY);

  m_statsPanel->setPosition(leftStartX, barY - 35.0f);

  float spellSize = 50.0f;
  float spellGap = 8.0f;
  float spellTotalW = 4 * spellSize + 3 * spellGap;
  m_spellsDisplay->setPosition(screenW / 2.0f - spellTotalW / 2.0f,
                               barY - 95.0f);

  float badgeRadius = 18.0f;
  m_levelBadge->setPosition(leftStartX + 160.0f - badgeRadius * 2 - 5.0f,
                            barY - 35.0f + 85.0f - badgeRadius * 2 - 5.0f);

  float xpBarLeft = leftStartX + 160.0f + 5.0f;
  // Le XPBar attend position et utilise ses dimensions internes
  // m_width/m_height
  m_xpBar->setPosition(xpBarLeft, barY - 30.0f);

  m_goldDisplay->setPosition(rightStartX, barY - 30.0f);
  m_shopButton->setPosition(rightStartX, barY - 5.0f);

  // --- RENDU ---
  m_hpBar->draw(window);
  m_mpBar->draw(window);
  m_xpBar->draw(window);
  m_statsPanel->draw(window);
  m_spellsDisplay->draw(window);
  m_levelBadge->draw(window);
  m_goldDisplay->draw(window);
  m_shopButton->draw(window);
}

bool HUD::isShopButtonClicked(const sf::Vector2f &mousePos) const {
  if (!m_shopButton)
    return false;
  return m_shopButton->getBounds().contains(mousePos);
}
