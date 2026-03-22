#pragma once
#include <string>
#include <vector>

enum class ItemCategory {
    DAMAGE,
    ATTACK_SPEED,
    DEFENSE,
    MAGIC
};

struct ItemStats {
    float ad = 0.0f;
    float as = 0.0f;
    float range = 0.0f;
    float hp = 0.0f;
    float mana = 0.0f;
};

struct ItemTemplate {
    int id;
    std::string name;
    std::string description;
    float price;
    bool isFinal;
    ItemCategory category;
    ItemStats stats;

    static std::vector<ItemTemplate> getShopItems();
};

inline std::vector<ItemTemplate> ItemTemplate::getShopItems() {
    return {
        {1, "Epee Longue", "+10 Degats d'attaque", 350.0f, false, ItemCategory::DAMAGE, {10.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
        {2, "Dague", "+12% Vitesse d'attaque", 300.0f, false, ItemCategory::ATTACK_SPEED, {0.0f, 0.12f, 0.0f, 0.0f, 0.0f}},
        {3, "Cristal de Saphir", "+250 Mana", 350.0f, false, ItemCategory::MAGIC, {0.0f, 0.0f, 0.0f, 0.0f, 250.0f}},
        {4, "Cristal de Rubis", "+150 PV", 400.0f, false, ItemCategory::DEFENSE, {0.0f, 0.0f, 0.0f, 150.0f, 0.0f}},
        {5, "Lame d'Infini", "+70 Degats d'attaque. (Final)", 3400.0f, true, ItemCategory::DAMAGE, {70.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
        {6, "Danseur Fantome", "+35% AS. (Final)", 2800.0f, true, ItemCategory::ATTACK_SPEED, {0.0f, 0.35f, 0.0f, 0.0f, 0.0f}},
        {7, "Force de la Trinite", "+30 AD, +30% AS, +300 HP. (Final)", 3333.0f, true, ItemCategory::DAMAGE, {30.0f, 0.30f, 0.0f, 300.0f, 0.0f}},
        {8, "Canon Ultrarapide", "+35% AS, +150 Range. (Final)", 3000.0f, true, ItemCategory::ATTACK_SPEED, {0.0f, 0.35f, 150.0f, 0.0f, 0.0f}},
        {9, "Armure de Warmog", "+800 PV. (Final)", 3000.0f, true, ItemCategory::DEFENSE, {0.0f, 0.0f, 0.0f, 800.0f, 0.0f}}
    };
}
