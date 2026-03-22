#pragma once
#include <string>

struct Spell {
    std::string name;
    std::string key;     // "A", "Z", "E", "R"
    int level = 0;       // 0 = non appris
    int maxLevel;        // 5 pour A/Z/E, 3 pour R

    float baseCooldown;  // Cooldown de base (diminue par niveau)
    float currentCooldown = 0.0f;
    float baseManaCost;  // Coût en mana de base

    // Cooldown effectif selon le niveau
    float getCooldown() const {
        if (level <= 0) return 0.0f;
        return baseCooldown - (level - 1) * 1.0f; // -1s par niveau
    }

    // Coût mana effectif selon le niveau
    float getManaCost() const {
        if (level <= 0) return 0.0f;
        return baseManaCost + (level - 1) * 10.0f; // +10 mana par niveau
    }

    bool isReady() const { return level > 0 && currentCooldown <= 0.0f; }

    bool canLevelUp() const { return level < maxLevel; }

    void levelUp() {
        if (level < maxLevel) ++level;
    }

    void use() {
        if (level > 0) currentCooldown = getCooldown();
    }

    void update(float dt) {
        if (currentCooldown > 0.0f) {
            currentCooldown -= dt;
            if (currentCooldown < 0.0f) currentCooldown = 0.0f;
        }
    }
};
