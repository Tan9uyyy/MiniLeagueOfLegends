# Mini League of Legends

Projet de reproduction en 2D d'un MOBA simplifié inspiré de League of Legends, développé en C++17 avec la bibliothèque SFML 3. Le projet intègre une architecture réseau client-serveur avec gestion de serveurs de jeu et d'un serveur maître (Master Server).

---

## 🛠️ Prérequis

- **Compilateur C++17** : GCC 11+, Clang 13+, ou MSVC 2022+
- **CMake** : Version 3.15 ou supérieure
- **SFML 3** : Modules Graphics, Window, System, Audio, Network

---

## 🚀 Compilation

Le projet utilise les [CMakePresets](CMakePresets.json) pour simplifier la configuration sous Linux et Windows.

### Sous Linux

```bash
# Configuration
cmake --preset linux

# Compilation
cmake --build --preset linux
```

Les exécutables générés se trouvent dans `build-linux/bin/` :
- `MiniLeagueOfLegends` (Client de jeu)
- `MiniLeagueOfLegendsServer` (Serveur de match)
- `MiniLeagueOfLegendsMasterServer` (Serveur maître / Lobby)

### Sous Windows (MinGW)

```bash
# Configuration
cmake --preset windows

# Compilation
cmake --build --preset windows
```

Les exécutables se trouveront dans `build-win/bin/`.

---

## 🎮 Lancement

1. **Lancer le serveur maître (optionnel selon le mode)** :
   ```bash
   ./build-linux/bin/MiniLeagueOfLegendsMasterServer
   ```
2. **Lancer le serveur de partie** :
   ```bash
   ./build-linux/bin/MiniLeagueOfLegendsServer
   ```
3. **Lancer un ou plusieurs clients** :
   ```bash
   ./build-linux/bin/MiniLeagueOfLegends
   ```

---

## 📁 Structure du projet

```
.
├── assets/             # Ressources multimédias (sprites, sons, polices)
├── include/            # En-têtes C++ (.hpp)
│   ├── HUD/            # Composants d'interface (Barres, sorts, lobby, etc.)
│   ├── nlohmann/       # Bibliothèque JSON
│   └── server/         # En-têtes serveur
├── src/                # Code source C++ (.cpp)
│   ├── HUD/            # Implémentations HUD et Lobby
│   └── server/         # Implémentations serveurs de jeu et maître
├── CMakeLists.txt      # Configuration CMake du projet
├── CMakePresets.json   # Presets de compilation Linux & Windows
├── .gitignore          # Fichiers et dossiers exclus du versionnement
└── .gitattributes     # Gestion des fins de ligne (LF)
```
