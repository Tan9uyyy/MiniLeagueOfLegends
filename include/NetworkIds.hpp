#pragma once
#include <cstdint>

namespace NetworkIds {
    // IDs pour les entités statiques du monde
    // Ces IDs doivent être identiques entre le Client et le Serveur
    static constexpr uint32_t MAP          = 0;
    static constexpr uint32_t ALLIED_NEXUS = 1;
    static constexpr uint32_t ALLIED_TURRET_1 = 2;
    static constexpr uint32_t ALLIED_TURRET_2 = 3;
    static constexpr uint32_t ENEMY_NEXUS  = 4;
    static constexpr uint32_t ENEMY_TURRET_1  = 5;
    static constexpr uint32_t ENEMY_TURRET_2  = 6;

    // ID de départ pour les entités dynamiques (Champions, etc.)
    static constexpr uint32_t START_DYNAMIC_ID = 100;
}
