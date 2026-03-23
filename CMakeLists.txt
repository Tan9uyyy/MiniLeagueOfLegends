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
find_package(SFML 3 COMPONENTS Graphics Window System Audio Network REQUIRED)

# Liste explicite des fichiers sources partagés
set(SHARED_SRC_FILES
    src/Champion.cpp
    src/Map.cpp
    src/Pathfinder.cpp
    src/ClickIndicator.cpp
    src/Nexus.cpp
    src/Turret.cpp
)

# Fichiers sources spécifiques au client
set(CLIENT_SRC_FILES
    src/main.cpp
    src/Game.cpp
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

# 1. Création de la cible principale (le client)
add_executable(${PROJECT_NAME} ${CLIENT_SRC_FILES} ${SHARED_SRC_FILES})

# 2. Ajout du dossier 'include'
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
    SFML::Network
)

# 5. Définition du dossier de sortie de l'exécutable
set_target_properties(${PROJECT_NAME} PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# --- CIBLE SERVEUR ---
set(SERVER_SRC_FILES
    src/server/ServerMain.cpp
    src/server/ServerGame.cpp
)

add_executable(${PROJECT_NAME}Server ${SERVER_SRC_FILES} ${SHARED_SRC_FILES})

target_include_directories(${PROJECT_NAME}Server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(${PROJECT_NAME}Server PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_link_libraries(${PROJECT_NAME}Server PRIVATE
    SFML::System
    SFML::Network
    SFML::Graphics
)

set_target_properties(${PROJECT_NAME}Server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)


# 6. Copier le dossier assets à côté de l'exécutable
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets 
     DESTINATION ${CMAKE_BINARY_DIR}/bin
)