#include "Champion.hpp"
#include "Map.hpp"
#include "Pathfinder.hpp"
#include <iostream>

Champion::Champion(sf::Vector2f startPosition, const Map &map, Team team)
    : CombatEntity(team, 1000.0f), m_speed(300.0f), m_isMovingToTarget(false),
      m_map(map) {
  m_shape.setRadius(20.0f);

  if (team == Team::ALLIED) {
    m_shape.setFillColor(sf::Color::Blue);
  } else {
    m_shape.setFillColor(sf::Color::Red);
  }

  m_shape.setOrigin(20.0f, 20.0f);
  m_shape.setPosition(startPosition);

  // Initialisation des 4 sorts
  m_spells[0] = {"Frappe Heroique", "A", 0, 5, 8.0f, 0.0f, 50.0f};
  m_spells[1] = {"Vague d'Energie", "Z", 0, 5, 10.0f, 0.0f, 70.0f};
  m_spells[2] = {"Charge",          "E", 0, 5, 12.0f, 0.0f, 60.0f};
  m_spells[3] = {"Ultime",          "R", 0, 3, 80.0f, 0.0f, 100.0f};
}

void Champion::setTargetPosition(sf::Vector2f target) {
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
      p.setOrigin(5.0f, 5.0f);
      p.setPosition(point);
      m_debugPathShapes.push_back(p);
    }
  }
}

void Champion::update(float deltaTime) {
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
      sf::Vector2f targetCenter(targetBounds.left + targetBounds.width / 2.0f,
                                targetBounds.top + targetBounds.height / 2.0f);
      sf::Vector2f myPos = getPosition();

      float dx = targetCenter.x - myPos.x;
      float dy = targetCenter.y - myPos.y;
      float distance = std::sqrt(dx * dx + dy * dy);

      // On prend en compte le rayon de l'entité cible pour la portée d'attaque
      // (attaquer le "bord")
      float effectiveRange =
          m_attackRange +
          std::max(targetBounds.width, targetBounds.height) / 2.0f;

      if (distance <= effectiveRange) {
        // À portée ! On s'arrête et on tape
        m_path.clear();
        m_isMovingToTarget = false;

        if (m_attackCooldown <= 0.0f) {
          m_target->takeDamage(m_attackDamage);
          m_attackCooldown = 1.0f / m_attackSpeed; // Reset du timer

          // Activer le visuel du laser
          m_isAttacking = true;
          m_attackVisualTimer = m_attackVisualDuration;

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
    sf::Vector2f targetCenter(targetBounds.left + targetBounds.width / 2.0f,
                              targetBounds.top + targetBounds.height / 2.0f);
    sf::Vector2f myPos = getPosition();

    sf::Vertex line[] = {sf::Vertex(myPos, sf::Color::Yellow),
                         sf::Vertex(targetCenter, sf::Color::Red)};
    window.draw(line, 2, sf::Lines);
  }

  // Dessiner la propre barre de vie du champion
  drawHealthBar(window, getPosition(), 40.0f, 5.0f);
}

sf::Vector2f Champion::getPosition() const { return m_shape.getPosition(); }

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

// --- Système de Sorts et Niveaux ---

void Champion::gainXP(int amount) {
    if (m_level >= 18) return;
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
        if (m_level < 18) {
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
    Spell& spell = m_spells[spellIndex];
    if (!spell.isReady()) return false;
    if (m_currentMana < spell.getManaCost()) return false;

    m_currentMana -= spell.getManaCost();
    spell.use();
    std::cout << "Sort " << spell.key << " lance! (CD: " << spell.getCooldown() << "s, Mana: " << spell.getManaCost() << ")" << std::endl;
    return true;
}
