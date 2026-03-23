#pragma once
#include "CombatEntity.hpp"
#include "Item.hpp"
#include "Map.hpp"
#include "Spell.hpp"
#include <SFML/Graphics.hpp>
#include <array>
#include <cmath>
#include <vector>

class Map; // Forward declaration

class Champion : public CombatEntity {
public:
  // Un champion a par défaut 1000 PV (exemple)
  Champion(sf::Vector2f startPosition, const Map &map,
           Team team = Team::ALLIED);

  void update(float deltaTime) override;
  void draw(sf::RenderWindow &window) override;

  // Surcharge de la méthode pour gérer le déplacement par clic
  void setTargetPosition(sf::Vector2f target) override;

  // Surcharge pour gérer les dégâts (interrompt le rappel)
  virtual void takeDamage(float amount) override;

  // Retourne la position actuelle du champion pour que la caméra puisse le
  // suivre
  sf::Vector2f getPosition() const;
  sf::FloatRect getBounds() const override { return m_shape.getGlobalBounds(); }

  // Définir une cible à attaquer
  void setTargetEntity(CombatEntity *target) { m_target = target; }

  // Inventaire
  const std::vector<ItemTemplate> &getInventory() const { return m_inventory; }
  int getInventoryIndexOf(int itemId) const;
  bool buyItem(const ItemTemplate &item);
  bool sellItem(int inventoryIndex);
  bool hasFinalItem(int itemId) const;
  bool canShop() const;

  // Accesseurs pour l'Interface Utilisateur (HUD)
  float getMana() const { return m_currentMana; }
  float getMaxMana() const { return m_maxMana; }
  float getGold() const { return m_gold; }
  float getAttackDamage() const { return m_attackDamage; }
  float getAttackSpeed() const { return m_attackSpeed; }
  float getAttackRange() const { return m_attackRange; }

  // Système de Sorts et Niveaux
  int getLevel() const { return m_level; }
  int getSkillPoints() const { return m_skillPoints; }
  const std::array<Spell, 4>& getSpells() const { return m_spells; }
  const Spell& getSpell(int index) const { return m_spells[index]; }
  bool upgradeSpell(int spellIndex);
  bool castSpell(int spellIndex);
  void gainXP(int amount);
  void debugLevelUp();
  int getXP() const { return m_xp; }
  int getXPToNextLevel() const { return m_xpToNextLevel; }

  // Sort Rappel (B)
  void startRecall();
  bool isRecalling() const { return m_isRecalling; }

private:
  std::vector<ItemTemplate> m_inventory; // Added inventory member
  sf::CircleShape m_shape; // Représentation temporaire de notre champion
  float m_speed;           // Vitesse de déplacement en pixels par seconde

  std::vector<sf::Vector2f> m_path; // Liste des points à parcourir
  bool m_isMovingToTarget;          // Vrai si le champion suit un chemin

  const Map &m_map; // Référence vers le monde pour les collisions

  // Attributs de combat
  CombatEntity *m_target = nullptr; // L'entité que l'on traque
  float m_attackRange = 150.0f;     // Portée (mêlée allongée)
  float m_attackDamage = 550.0f;    // Dégâts par coup
  float m_attackSpeed = 1.0f;       // 1 attaque par seconde
  float m_attackCooldown = 0.0f;    // Temps avant la prochaine attaque

  // Variables pour le visuel de l'attaque
  bool m_isAttacking = false;
  float m_attackVisualTimer = 0.0f;
  float m_attackVisualDuration =
      0.15f; // Le laser s'affiche pendant 0.15 secondes

  // Système d'économie
  float m_gold = 500.0f;
  float m_matchTime = 0.0f;
  float m_printTimer =
      0.0f; // Timer pour n'imprimer dans la console qu'une fois par seconde

  // Système de Mana
  float m_maxMana = 500.0f;
  float m_currentMana = 500.0f;

  // Système de Sorts et Niveaux
  std::array<Spell, 4> m_spells;
  int m_level = 1;
  int m_xp = 0;
  int m_xpToNextLevel = 280; // XP nécessaire pour le niveau 2
  int m_skillPoints = 1;     // 1 point de compétence au niveau 1

  // Table XP par niveau (LoL-like)
  static constexpr int XP_TABLE[18] = {
    0, 280, 380, 480, 580, 680, 780, 880, 980, 1080,
    1180, 1280, 1380, 1480, 1580, 1680, 1780, 1880
  };

  // (Optionnel) Pour le débogage : afficher le chemin recalculé
  std::vector<sf::CircleShape> m_debugPathShapes;

  // Variables du Rappel
  bool m_isRecalling = false;
  float m_recallTimer = 0.0f;
  static constexpr float RECALL_DURATION = 8.0f;
};
