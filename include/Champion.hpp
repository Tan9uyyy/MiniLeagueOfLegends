#pragma once
#include "CombatEntity.hpp"
#include "Item.hpp"
#include "Map.hpp"
#include "Spell.hpp"
#include <SFML/Graphics.hpp>
#include "Config.hpp"
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
  sf::Vector2f getPosition() const override;
  void setPosition(sf::Vector2f pos) override;

  sf::FloatRect getBounds() const override { return m_shape.getGlobalBounds(); }

  // Définir une cible à attaquer
  void setTargetEntity(CombatEntity *target) { m_target = target; }

  // Exécute l'animation d'attaque localement (appelé par le client après un événement serveur)
  void playAttackAnimation(CombatEntity* target);
  // Met à jour les compteurs visuels (appelé sur le client)
  void updateVisuals(float deltaTime);
  // Le serveur vérifie si une attaque vient d'avoir lieu et récupère l'ID de la cible
  bool popJustAttacked(uint32_t& outTargetId);

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

  // Setters pour synchronisation Réseau
  void setGold(float gold) { m_gold = gold; }
  void setMana(float mana) { m_currentMana = mana; }
  void setMaxMana(float maxMana) { m_maxMana = maxMana; }
  void setIsRecalling(bool r) { m_isRecalling = r; }
  void setRecallTimer(float t) { m_recallTimer = t; }
  float getRecallTimer() const { return m_recallTimer; }

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
  bool m_justAttacked = false;
  uint32_t m_justAttackedTargetId = 0;

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
  static constexpr int XP_TABLE[Config::Champion::MAX_LEVEL] = {
    0, 280, 380, 480, 580, 680, 780, 880, 980, 1080,
    1180, 1280, 1380, 1480, 1580, 1680, 1780, 1880
  };

  // (Optionnel) Pour le débogage : afficher le chemin recalculé
  std::vector<sf::CircleShape> m_debugPathShapes;

  // Variables du Rappel
  bool m_isRecalling = false;
  float m_recallTimer = 0.0f;

  // Respawn
  float m_respawnTimer = 0.0f;
  static constexpr float RESPAWN_DURATION = 10.0f;
};
