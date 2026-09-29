#include "Champion.hpp"
#include "Config.hpp"
#include "Map.hpp"
#include "Pathfinder.hpp"
#include <iostream>

Champion::Champion(sf::Vector2f startPosition, const Map &map, Team team)
    : CombatEntity(team, 1000.0f), m_speed(Config::Champion::SPEED), m_isMovingToTarget(false),
      m_map(map) {
  m_shape.setRadius(Config::Champion::RADIUS);

  if (team == Team::ALLIED) {
    m_shape.setFillColor(sf::Color::Blue);
  } else {
    m_shape.setFillColor(sf::Color::Red);
  }

  m_shape.setOrigin({Config::Champion::RADIUS, Config::Champion::RADIUS});
  m_shape.setPosition(startPosition);

  // Initialisation des 4 sorts
  m_spells[0] = {"Frappe Heroique", "A", 0, 5, 8.0f, 0.0f, 50.0f};
  m_spells[1] = {"Vague d'Energie", "Z", 0, 5, 10.0f, 0.0f, 70.0f};
  m_spells[2] = {"Charge",          "E", 0, 5, 12.0f, 0.0f, 60.0f};
  m_spells[3] = {"Ultime",          "R", 0, 3, 80.0f, 0.0f, 100.0f};
}

void Champion::setTargetPosition(sf::Vector2f target) {
  // Interrompre le rappel si on bouge
  if (m_isRecalling) {
    m_isRecalling = false;
    std::cout << "Rappel interrompu (deplacement)" << std::endl;
  }

  // Si on clique au sol (appel normal depuis main.cpp), on annule la cible
  // d'attaque
  m_target = nullptr;

  // Demander le chemin à l'astucieux Pathfinder !
  // On lui donne le rayon du champion pour qu'il évite de frôler les murs de
  // trop près
  m_path = Pathfinder::findPath(m_map, m_shape.getPosition(), target,
                                m_shape.getRadius());

  if (!m_path.empty()) {
    m_isMovingToTarget = true;

    // --- Optionnel : Créer des ronds rouges pour débuguer le chemin visualisé
    // ---
    m_debugPathShapes.clear();
    for (const auto &point : m_path) {
      sf::CircleShape p(5.0f);
      p.setFillColor(sf::Color::Red);
      p.setOrigin({5.0f, 5.0f});
      p.setPosition(point);
      m_debugPathShapes.push_back(p);
    }
  }
}

void Champion::update(float deltaTime) {
  if (isDead()) {
    m_target = nullptr;
    m_path.clear();
    m_isMovingToTarget = false;
    m_isRecalling = false;
    m_isAttacking = false;

    m_respawnTimer -= deltaTime;
    if (m_respawnTimer <= 0.0f) {
      m_currentHealth = m_maxHealth;
      m_currentMana = m_maxMana;
      setPosition(m_map.getSpawnPosition(getTeam()));
      std::cout << "Champion a respawn!" << std::endl;
    }
    return; // Stop update logic if dead
  }

  // Gestion du rappel
  if (m_isRecalling) {
    m_recallTimer -= deltaTime;
    if (m_recallTimer <= 0.0f) {
      m_isRecalling = false;
      m_shape.setPosition(m_map.getSpawnPosition(getTeam()));
      // Reset path/target to be safe
      m_path.clear();
      m_isMovingToTarget = false;
      m_target = nullptr;
      std::cout << "Rappel termine : retour a la base" << std::endl;
    }
  }

  // Gestion du chronomètre et de l'or
  m_matchTime += deltaTime;

  // Si la partie a dépassé 1min50s (110 secondes), on active le gain passif
  // (20.4 toutes les 10s = 2.04 / sec)
  if (m_matchTime >= 110.0f) {
    m_gold += 2.04f * deltaTime;
  }

  // Affichage dans la console une fois par seconde
  m_printTimer += deltaTime;
  if (m_printTimer >= 1.0f) {
    m_printTimer -= 1.0f;
    std::cout << "[TEMPS] " << (int)m_matchTime / 60 << "m "
              << (int)m_matchTime % 60 << "s | [OR] " << (int)m_gold << " G"
              << std::endl;
  }

  // Gestion du cooldown d'attaque
  if (m_attackCooldown > 0.0f) {
    m_attackCooldown -= deltaTime;
  }

  // Gestion du timer visuel d'attaque
  if (m_isAttacking) {
    m_attackVisualTimer -= deltaTime;
    if (m_attackVisualTimer <= 0.0f) {
      m_isAttacking = false;
    }
  }

  // Mise à jour des cooldowns des sorts
  for (auto& spell : m_spells) {
    spell.update(deltaTime);
  }

  // Gestion du ciblage (Auto-Attack)
  if (m_target) {
    if (m_target->isDead()) {
      m_target = nullptr;
      m_path.clear();
      m_isMovingToTarget = false;
      // Gain d'XP quand on tue une entité
      gainXP(200);
    } else {
      // Calcul de la distance avec la cible
      sf::FloatRect targetBounds = m_target->getBounds();
      sf::Vector2f targetCenter(targetBounds.position.x + targetBounds.size.x / 2.0f,
                                targetBounds.position.y + targetBounds.size.y / 2.0f);
      sf::Vector2f myPos = getPosition();

      float dx = targetCenter.x - myPos.x;
      float dy = targetCenter.y - myPos.y;
      float distance = std::sqrt(dx * dx + dy * dy);

      // On prend en compte le rayon de l'entité cible pour la portée d'attaque
      // (attaquer le "bord")
      float effectiveRange =
          m_attackRange +
          std::max(targetBounds.size.x, targetBounds.size.y) / 2.0f;

      if (distance <= effectiveRange) {
        // À portée ! On s'arrête et on tape
        m_path.clear();
        m_isMovingToTarget = false;

        if (m_attackCooldown <= 0.0f) {
          m_target->takeDamage(m_attackDamage);
          m_attackCooldown = 1.0f / m_attackSpeed; // Reset du timer

          // Activer le visuel du laser et notifier le serveur
          m_isAttacking = true;
          m_attackVisualTimer = m_attackVisualDuration;
          m_justAttacked = true;
          m_justAttackedTargetId = m_target->getNetworkId();

          std::cout << "Champion attacks for " << m_attackDamage
                    << " damage! Target HP: " << m_target->getHealth()
                    << std::endl;
        }
      } else if (!m_isMovingToTarget) {
        // Trop loin et pas déjà en train de bouger ? Poursuivre !
        // On met temporairement l'objectif sans réinitialiser m_target
        auto storedTarget = m_target;
        setTargetPosition(targetCenter);
        m_target = storedTarget; // Restaurer la cible que setTargetPosition
                                 // vient d'annuler
      }
    }
  }

  if (m_isMovingToTarget && !m_path.empty()) {
    sf::Vector2f currentPos = m_shape.getPosition();
    sf::Vector2f currentTarget =
        m_path.front(); // Le prochain point à atteindre

    sf::Vector2f direction = currentTarget - currentPos;
    float distanceToNextPoint =
        std::sqrt(direction.x * direction.x + direction.y * direction.y);

    // Si on est proche du point de passage
    if (distanceToNextPoint < 5.0f) {
      // On le supprime de la liste
      m_path.erase(m_path.begin());

      // Si c'était le dernier point, on s'arrête
      if (m_path.empty()) {
        m_isMovingToTarget = false;
        m_debugPathShapes.clear();
      }
    } else {
      // Sinon, on avance vers ce point de passage
      direction.x /= distanceToNextPoint;
      direction.y /= distanceToNextPoint;

      sf::Vector2f velocity = direction * m_speed * deltaTime;
      m_shape.move(velocity);
    }
  }
}

void Champion::draw(sf::RenderWindow &window) {
  if (isDead())
    return;

  // Dessin du chemin pour le débogage
  for (const auto &p : m_debugPathShapes) {
    window.draw(p);
  }

  // Le champion par dessus le chemin
  window.draw(m_shape);

  // Dessin du laser d'attaque si actif
  if (m_isAttacking && m_target) {
    sf::FloatRect targetBounds = m_target->getBounds();
    sf::Vector2f targetCenter(targetBounds.position.x + targetBounds.size.x / 2.0f,
                              targetBounds.position.y + targetBounds.size.y / 2.0f);
    sf::Vector2f myPos = getPosition();

    sf::Vertex line[] = {{myPos, sf::Color::Yellow},
                         {targetCenter, sf::Color::Red}};
    window.draw(line, 2, sf::PrimitiveType::Lines);
  }

  // Dessiner la propre barre de vie du champion
  drawHealthBar(window, getPosition(), 40.0f, 5.0f);

  // Dessiner la barre de progression du rappel
  if (m_isRecalling) {
    float progress = 1.0f - (m_recallTimer / Config::Champion::RECALL_DURATION); // 0 à 1
    float barWidth = 50.0f;
    float barHeight = 4.0f;
    sf::Vector2f pos = getPosition();
    
    sf::RectangleShape bgBar(sf::Vector2f(barWidth, barHeight));
    bgBar.setFillColor(sf::Color(0, 0, 0, 150));
    bgBar.setPosition({pos.x - barWidth / 2.0f, pos.y - 30.0f});

    sf::RectangleShape progBar(sf::Vector2f(barWidth * progress, barHeight));
    progBar.setFillColor(sf::Color(0, 150, 255)); // Bleu clair
    progBar.setPosition({pos.x - barWidth / 2.0f, pos.y - 30.0f});

    window.draw(bgBar);
    window.draw(progBar);
  }
}

sf::Vector2f Champion::getPosition() const { return m_shape.getPosition(); }
void Champion::setPosition(sf::Vector2f pos) { m_shape.setPosition(pos); }

bool Champion::canShop() const {
  return m_map.isInSpawnArea(getPosition(), getTeam());
}

bool Champion::hasFinalItem(int itemId) const {
    for (const auto& item : m_inventory) {
        if (item.id == itemId && item.isFinal) return true;
    }
    return false;
}

int Champion::getInventoryIndexOf(int itemId) const {
    for (size_t i = 0; i < m_inventory.size(); ++i) {
        if (m_inventory[i].id == itemId) return i;
    }
    return -1;
}

bool Champion::buyItem(const ItemTemplate& item) {
    if (m_gold < item.price) return false;
    if (m_inventory.size() >= 6) return false;
    if (item.isFinal && hasFinalItem(item.id)) return false;

    m_gold -= item.price;
    m_inventory.push_back(item);
    
    // Add stats
    m_attackDamage += item.stats.ad;
    m_attackSpeed += item.stats.as;
    m_attackRange += item.stats.range;
    
    m_maxHealth += item.stats.hp;
    m_currentHealth += item.stats.hp; // So you heal when you buy HP!
    
    m_maxMana += item.stats.mana;
    m_currentMana += item.stats.mana;
    
    return true;
}

bool Champion::sellItem(int inventoryIndex) {
    if (inventoryIndex < 0 || inventoryIndex >= (int)m_inventory.size()) return false;
    
    ItemTemplate item = m_inventory[inventoryIndex];
    m_gold += item.price * 0.7f;
    
    m_attackDamage -= item.stats.ad;
    m_attackSpeed -= item.stats.as;
    m_attackRange -= item.stats.range;
    
    m_maxHealth -= item.stats.hp;
    m_currentHealth = std::min(m_currentHealth, m_maxHealth);
    
    m_maxMana -= item.stats.mana;
    m_currentMana = std::min(m_currentMana, m_maxMana);

    m_inventory.erase(m_inventory.begin() + inventoryIndex);
    return true;
}

void Champion::regen(float hpPercent, float manaPercent, float deltaTime) {
    if (isDead()) return;
    
    float regenHP = m_maxHealth * hpPercent * deltaTime;
    float regenMP = m_maxMana * manaPercent * deltaTime;
    
    m_currentHealth = std::min(m_maxHealth, m_currentHealth + regenHP);
    m_currentMana = std::min(m_maxMana, m_currentMana + regenMP);
}

void Champion::takeDamage(float amount) {
    bool wasDead = isDead();
    CombatEntity::takeDamage(amount);
    
    // Interrompre le rappel si on subit des dégâts (uniquement si le montant est > 0 pour être sûr)
    if (m_isRecalling && amount > 0.0f) {
        m_isRecalling = false;
        std::cout << "Rappel interrompu (degats subis) !" << std::endl;
    }

    if (!wasDead && isDead()) {
        m_respawnTimer = RESPAWN_DURATION;
        std::cout << "Champion est mort ! Respawn dans " << RESPAWN_DURATION << "s..." << std::endl;
    }
}

void Champion::startRecall() {
    if (m_isRecalling) return; // Déjà en cours
    
    m_isRecalling = true;
    m_recallTimer = Config::Champion::RECALL_DURATION;
    
    // Annuler les actions en cours
    m_path.clear();
    m_isMovingToTarget = false;
    m_target = nullptr;
    m_isAttacking = false;
    
    std::cout << "Canalisation du Rappel (8s)..." << std::endl;
}

// --- Système de Sorts et Niveaux ---

void Champion::gainXP(int amount) {
    if (m_level >= Config::Champion::MAX_LEVEL) return;
    m_xp += amount;
    while (m_level < 18 && m_xp >= m_xpToNextLevel) {
        m_xp -= m_xpToNextLevel;
        m_level++;
        m_skillPoints++;
        // Stats par niveau
        m_maxHealth += 80.0f;
        m_currentHealth += 80.0f;
        m_maxMana += 30.0f;
        m_currentMana += 30.0f;
        m_attackDamage += 3.0f;
        // XP pour le prochain niveau
        if (m_level < Config::Champion::MAX_LEVEL) {
            m_xpToNextLevel = XP_TABLE[m_level];
        }
        std::cout << "LEVEL UP! Niveau " << m_level << " (" << m_skillPoints << " point(s) disponible(s))" << std::endl;
    }
}

void Champion::debugLevelUp() {
    gainXP(m_xpToNextLevel);
}

bool Champion::upgradeSpell(int spellIndex) {
    if (spellIndex < 0 || spellIndex >= 4) return false;
    if (m_skillPoints <= 0) return false;
    if (!m_spells[spellIndex].canLevelUp()) return false;

    // R (index 3) nécessite les niveaux 6, 11, 16
    if (spellIndex == 3) {
        int requiredLevel[] = {6, 11, 16};
        int currentSpellLevel = m_spells[3].level;
        if (currentSpellLevel >= 3) return false;
        if (m_level < requiredLevel[currentSpellLevel]) return false;
    }

    m_spells[spellIndex].levelUp();
    m_skillPoints--;
    std::cout << "Sort " << m_spells[spellIndex].key << " ameliore au niveau " << m_spells[spellIndex].level << std::endl;
    return true;
}

bool Champion::castSpell(int spellIndex) {
    if (spellIndex < 0 || spellIndex >= 4) return false;
    
    // Interrompre le rappel si on lance un sort
    if (m_isRecalling) {
        m_isRecalling = false;
        std::cout << "Rappel interrompu (lancement d'un sort) !" << std::endl;
    }

    Spell& spell = m_spells[spellIndex];
    if (!spell.isReady()) return false;
    if (m_currentMana < spell.getManaCost()) return false;

    m_currentMana -= spell.getManaCost();
    spell.use();
    std::cout << "Sort " << spell.key << " lance! (CD: " << spell.getCooldown() << "s, Mana: " << spell.getManaCost() << ")" << std::endl;
    return true;
}

bool Champion::popJustAttacked(uint32_t& outTargetId) {
    if (m_justAttacked) {
        outTargetId = m_justAttackedTargetId;
        m_justAttacked = false;
        return true;
    }
    return false;
}

void Champion::playAttackAnimation(CombatEntity* target) {
    m_target = target;
    m_isAttacking = true;
    m_attackVisualTimer = m_attackVisualDuration;
}

void Champion::updateVisuals(float deltaTime) {
    if (m_isAttacking) {
        m_attackVisualTimer -= deltaTime;
        if (m_attackVisualTimer <= 0.0f) {
            m_isAttacking = false;
        }
    }
}
