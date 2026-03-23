#pragma once
#include "Entity.hpp"
#include "Team.hpp"

// Classe de base pour toutes les entités qui ont des points de vie (Champions,
// Sbires, Bâtiments)
class CombatEntity : public Entity {
public:
  CombatEntity(Team team, float maxHealth)
      : m_team(team), m_maxHealth(maxHealth), m_currentHealth(maxHealth) {}

  // Implémentation par défaut vide pour ceux qui ne bougent pas (ex: Bâtiments)
  void setTargetPosition(sf::Vector2f /*target*/) override {}

  Team getTeam() const { return m_team; }
  float getHealth() const { return m_currentHealth; }
  float getMaxHealth() const { return m_maxHealth; }
  bool isDead() const { return m_currentHealth <= 0.0f; }

  virtual void takeDamage(float amount) {
    m_currentHealth -= amount;
    if (m_currentHealth < 0.0f)
      m_currentHealth = 0.0f;
  }

  void setHealth(float hp) { m_currentHealth = hp; }
  void setMaxHealth(float maxHp) { m_maxHealth = maxHp; }

  // Obligatoire pour pouvoir cliquer sur une entité
  virtual sf::FloatRect getBounds() const override = 0;

protected:
  Team m_team;
  float m_maxHealth;
  float m_currentHealth;

  // Fonction utilitaire pour dessiner une barre de vie au-dessus de l'entité
  void drawHealthBar(sf::RenderWindow &window, sf::Vector2f position,
                     float width = 50.0f, float height = 5.0f) {
    if (isDead() || m_currentHealth >= m_maxHealth - 0.1f)
      return; // Optionnel : cacher si full vie

    // Fond (Rouge/Noir)
    sf::RectangleShape bgLine(sf::Vector2f(width, height));
    bgLine.setFillColor(sf::Color(50, 50, 50, 200));
    bgLine.setPosition({position.x - width / 2.0f,
                       position.y - 20.0f}); // Au-dessus du centre

    // Progression de la vie
    float healthPct = m_currentHealth / m_maxHealth;
    sf::RectangleShape healthLine(sf::Vector2f(width * healthPct, height));

    if (m_team == Team::ALLIED) {
      healthLine.setFillColor(sf::Color::Green);
    } else {
      healthLine.setFillColor(sf::Color::Red);
    }

    healthLine.setPosition({position.x - width / 2.0f, position.y - 20.0f});

    window.draw(bgLine);
    window.draw(healthLine);
  }
};
