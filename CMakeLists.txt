cmake_minimum_required(VERSION 3.15)

# Nom du projet et langage
project(MiniLeagueOfLegends VERSION 1.0 LANGUAGES CXX)

# Génère le fichier compile_commands.json (très utile pour les IDE/LSP)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Configuration globale du standard C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Recherche de SFML
find_package(SFML 3 COMPONENTS Graphics Window System Audio REQUIRED)

# Liste explicite des fichiers sources (La bonne pratique en CMake)
set(SRC_FILES
    src/main.cpp
    src/Game.cpp
    src/Champion.cpp
    src/Map.cpp
    src/Pathfinder.cpp
    src/ClickIndicator.cpp
    src/Nexus.cpp
    src/Turret.cpp
    src/HUD/HUD.cpp
    src/HUD/HUDBar.cpp
    src/HUD/HUDStats.cpp
    src/HUD/HUDSpells.cpp
    src/HUD/HUDLevel.cpp
    src/HUD/HUDGold.cpp
    src/ShopUI.cpp
    src/SettingsUI.cpp
    src/UIWindow.cpp
    src/Renderer.cpp
)

# 1. Création de la cible principale (l'exécutable)
add_executable(${PROJECT_NAME} ${SRC_FILES})

# 2. Ajout du dossier 'include' uniquement à cette cible
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 3. Ajout des options de compilation (Warnings) uniquement à cette cible
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# 4. Lien avec les bibliothèques SFML
target_link_libraries(${PROJECT_NAME} PRIVATE 
    SFML::Graphics 
    SFML::Window 
    SFML::System 
    SFML::Audio
)

# 5. Définition du dossier de sortie de l'exécutable
set_target_properties(${PROJECT_NAME} PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# 6. Copier le dossier assets à côté de l'exécutable
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets 
     DESTINATION ${CMAKE_BINARY_DIR}/bin
)