#include "Game.hpp"
#include "Config.hpp"
#include "ClickIndicator.hpp"
#include "NetworkIds.hpp"
#include "NetworkMessages.hpp"
#include "Nexus.hpp"
#include "Turret.hpp"
#include <iostream>

Game::Game() {
  // 1. Fenêtre en plein écran
  sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
  m_window.create(desktopMode, Config::Game::WINDOW_TITLE, sf::State::Fullscreen);
  m_window.setFramerateLimit(Config::Game::FPS_LIMIT);

  // 2. Vues (caméra + minimap)
  initViews();

  // 3. Police d'écriture
  m_fontLoaded =
      m_font.openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf") ||
      m_font.openFromFile(
          "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf") ||
      m_font.openFromFile("/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf");

  // 4. Initialisation Réseau (Rejoindre le serveur)
  initNetwork();

  // 5. Monde (Désormais on instancie peu de choses localement)
  initWorld();

  // 5. Interface
  m_hud = std::make_unique<HUD>(m_champion, m_font);
  m_shopUI = std::make_unique<ShopUI>(m_champion, m_font);
  m_settingsUI = std::make_unique<SettingsUI>(m_font);

  // Enregistrer les fenêtres UI (ordre = priorité d'interception, du plus
  // prioritaire au moins)
  m_uiWindows.push_back(m_settingsUI.get());
  m_uiWindows.push_back(m_shopUI.get());
}

void Game::initViews() {
  sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();

  m_camera = sf::View(sf::Vector2f(Config::Game::MAP_WIDTH / 2.0f, Config::Game::MAP_HEIGHT / 2.0f),
                      sf::Vector2f(desktopMode.size.x, desktopMode.size.y));

  m_minimapView = sf::View(sf::Vector2f(Config::Game::MAP_WIDTH / 2.0f, Config::Game::MAP_HEIGHT / 2.0f),
                           sf::Vector2f(Config::Game::MAP_WIDTH, Config::Game::MAP_HEIGHT));

  float minimapSize = Config::Game::MINIMAP_SIZE;
  float viewportW = minimapSize / desktopMode.size.x;
  float viewportH = minimapSize / desktopMode.size.y;

  m_minimapView.setViewport(
      sf::FloatRect({1.0f - viewportW - 0.01f, 1.0f - viewportH - 0.01f},
                    {viewportW, viewportH}));
}

void Game::initNetwork() {
  m_socket.setBlocking(true); // Bloquant pour attendre la réponse initiale

  sf::Packet joinPacket;
  joinPacket << MessageType::JOIN;
  auto ipStr = sf::IpAddress::resolve(Config::Network::SERVER_IP);
  if (!ipStr || m_socket.send(joinPacket, ipStr.value(), Config::Network::SERVER_PORT) !=
                    sf::Socket::Status::Done) {
    std::cerr << "Erreur : Impossible d'envoyer le paquet JOIN au serveur."
              << std::endl;
    return;
  }

  std::cout << "En attente de connexion au serveur..." << std::endl;

  sf::Packet reply;
  std::optional<sf::IpAddress> senderIp;
  unsigned short senderPort;

  // On attend jusqu'à recevoir JOIN_ACK
  while (true) {
    if (m_socket.receive(reply, senderIp, senderPort) ==
            sf::Socket::Status::Done &&
        senderIp.has_value()) {
      MessageType type;
      if (reply >> type && type == MessageType::JOIN_ACK) {
        reply >> m_localChampionId;
        std::cout << "Connecte avec l'ID Champion : " << m_localChampionId
                  << std::endl;
        break;
      }
    }
  }

  m_socket.setBlocking(
      false); // On repasse en non bloquant pour la boucle de jeu
}

void Game::initWorld() {
  auto mapPtr = std::make_unique<Map>(Config::Game::MAP_WIDTH, Config::Game::MAP_HEIGHT);
  m_gameMap = mapPtr.get();

  // Nexus Allié
  auto alliedNexus =
      std::make_unique<Nexus>(Config::Map::ALLIED_NEXUS_POS, Team::ALLIED);
  m_gameMap->addObstacle(alliedNexus->getBounds());
  alliedNexus->setNetworkId(NetworkIds::ALLIED_NEXUS);
  m_entities.push_back(std::move(alliedNexus));

  // Tourelles Alliées
  auto turretA1 =
      std::make_unique<Turret>(Config::Map::ALLIED_TURRET_1_POS, Team::ALLIED);
  auto turretA2 =
      std::make_unique<Turret>(Config::Map::ALLIED_TURRET_2_POS, Team::ALLIED);
  m_gameMap->addObstacle(turretA1->getBounds());
  m_gameMap->addObstacle(turretA2->getBounds());
  turretA1->setNetworkId(NetworkIds::ALLIED_TURRET_1);
  turretA2->setNetworkId(NetworkIds::ALLIED_TURRET_2);
  m_entities.push_back(std::move(turretA1));
  m_entities.push_back(std::move(turretA2));

  // Nexus Ennemi
  auto enemyNexus =
      std::make_unique<Nexus>(Config::Map::ENEMY_NEXUS_POS, Team::ENEMY);
  m_gameMap->addObstacle(enemyNexus->getBounds());
  enemyNexus->setNetworkId(NetworkIds::ENEMY_NEXUS);
  m_entities.push_back(std::move(enemyNexus));

  // Tourelles Ennemies
  auto turretE1 =
      std::make_unique<Turret>(Config::Map::ENEMY_TURRET_1_POS, Team::ENEMY);
  auto turretE2 =
      std::make_unique<Turret>(Config::Map::ENEMY_TURRET_2_POS, Team::ENEMY);
  m_gameMap->addObstacle(turretE1->getBounds());
  m_gameMap->addObstacle(turretE2->getBounds());
  turretE1->setNetworkId(NetworkIds::ENEMY_TURRET_1);
  turretE2->setNetworkId(NetworkIds::ENEMY_TURRET_2);
  m_entities.push_back(std::move(turretE1));
  m_entities.push_back(std::move(turretE2));

  // Carte (en fond) et indicateur
  m_entities.push_back(std::move(mapPtr));
  m_entities.push_back(std::make_unique<ClickIndicator>());

  // On crée un champion local "Dummy" pour la caméra et l'interface
  // Sa vraie position sera mise à jour par le serveur.
  auto championPtr = std::make_unique<Champion>(Config::Map::ALLIED_SPAWN_POS,
                                                *m_gameMap, Team::ALLIED);
  championPtr->setNetworkId(m_localChampionId);
  m_champion = championPtr.get();
  m_entities.push_back(std::move(championPtr));
}

void Game::run() {
  sf::Clock clock;
  while (m_window.isOpen()) {
    float deltaTime = clock.restart().asSeconds();
    processEvents();
    update(deltaTime);
    render();
  }
}

// --- Factorisation : une seule boucle sur toutes les fenêtres UI ---
bool Game::dispatchMouseEventToWindows(const sf::Event &event) {
  bool isMouseEvent = (event.is<sf::Event::MouseButtonPressed>() ||
                       event.is<sf::Event::MouseButtonReleased>() ||
                       event.is<sf::Event::MouseMoved>());
  if (!isMouseEvent)
    return false;

  sf::Vector2i pixelPos;
  if (const auto *mouseMoved = event.getIf<sf::Event::MouseMoved>()) {
    pixelPos = sf::Vector2i(mouseMoved->position);
  } else if (const auto *mousePressed =
                 event.getIf<sf::Event::MouseButtonPressed>()) {
    pixelPos = sf::Vector2i(mousePressed->position);
  } else if (const auto *mouseReleased =
                 event.getIf<sf::Event::MouseButtonReleased>()) {
    pixelPos = sf::Vector2i(mouseReleased->position);
  }

  for (auto *win : m_uiWindows) {
    if (win && win->isOpen()) {
      UIAction action = win->handleEvent(event, pixelPos, m_window);
      if (action == UIAction::QUIT) {
        m_window.close();
        return true;
      }
      if (action == UIAction::CONSUMED) {
        return true;
      }
    }
  }

  return false;
}

void Game::processEvents() {
  while (const auto event = m_window.pollEvent()) {
    if (event->is<sf::Event::Closed>())
      m_window.close();

    // Raccourcis clavier
    if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
      if (keyPressed->code == sf::Keyboard::Key::Escape && m_settingsUI)
        m_settingsUI->toggle();
      if (keyPressed->code == sf::Keyboard::Key::P && m_shopUI)
        m_shopUI->toggle();

      // Debug : Level Up avec L
      if (keyPressed->code == sf::Keyboard::Key::L && m_champion)
        m_champion->debugLevelUp();

      // Rappel (B)
      if (keyPressed->code == sf::Keyboard::Key::B && m_champion) {
        sendSpell(4);
      }

      // Sorts : Ctrl+Touche = upgrade, Touche seule = cast
      if (m_champion) {
        bool ctrl = keyPressed->control;
        if (keyPressed->code == sf::Keyboard::Key::A) {
          if (ctrl)
            m_champion->upgradeSpell(0);
          else
            sendSpell(0);
        }
        if (keyPressed->code == sf::Keyboard::Key::Z) {
          if (ctrl)
            m_champion->upgradeSpell(1);
          else
            sendSpell(1);
        }
        if (keyPressed->code == sf::Keyboard::Key::E) {
          if (ctrl)
            m_champion->upgradeSpell(2);
          else
            sendSpell(2);
        }
        if (keyPressed->code == sf::Keyboard::Key::R) {
          if (ctrl)
            m_champion->upgradeSpell(3);
          else
            sendSpell(3);
        }
      }
    }

    // Dispatch souris aux fenêtres UI (factorisé !)
    if (dispatchMouseEventToWindows(*event))
      continue;

    // Clic gauche sur le bouton BOUTIQUE du HUD
    if (const auto *mousePressed =
            event->getIf<sf::Event::MouseButtonPressed>()) {
      if (mousePressed->button == sf::Mouse::Button::Left) {
        sf::Vector2i pixelPos(mousePressed->position);
        sf::Vector2f uiPos =
            m_window.mapPixelToCoords(pixelPos, m_window.getDefaultView());
        if (m_hud && m_hud->isShopButtonClicked(uiPos)) {
          if (m_shopUI)
            m_shopUI->toggle();
          continue;
        }
      }

      // Clic droit dans le monde
      if (mousePressed->button == sf::Mouse::Button::Right) {
        sf::Vector2i pixelPos(mousePressed->position);
        sf::Vector2f worldPos = m_window.mapPixelToCoords(pixelPos, m_camera);

        bool enemyClicked = false;

        for (auto &entity : m_entities) {
          if (auto combatEntity = dynamic_cast<CombatEntity *>(entity.get())) {
            if (combatEntity->getTeam() != Team::ALLIED &&
                combatEntity->getBounds().contains(worldPos) &&
                !combatEntity->isDead()) {
              sendAttack(combatEntity->getNetworkId());
              enemyClicked = true;
              break;
            }
          }
        }

        // On n'envoie que la requête au serveur, pas de pathfinding local !
        for (auto &entity : m_entities) {
          if (enemyClicked && entity.get() == m_champion)
            continue;
          // Localement, on dessine l'indicateur de clic s'il y en a un
          if (dynamic_cast<ClickIndicator *>(entity.get())) {
            entity->setTargetPosition(worldPos);
          }
        }

        if (!enemyClicked) {
          sendMove(worldPos.x, worldPos.y);
        }
      }
    }
  }
}

void Game::processNetwork() {
  sf::Packet packet;
  std::optional<sf::IpAddress> senderIp;
  unsigned short senderPort;

  while (m_socket.receive(packet, senderIp, senderPort) ==
             sf::Socket::Status::Done &&
         senderIp.has_value()) {
    MessageType type;
    if (packet >> type) {
      if (type == MessageType::STATE) {
        uint32_t entityCount;
      packet >> entityCount;

      for (uint32_t i = 0; i < entityCount; ++i) {
        uint8_t entityType;
        packet >> entityType;

        if (entityType == 1) { // Champion
          uint32_t id;
          float x, y, hp, maxHp, mana, maxMana, gold, recallTimer;
          bool recalling;
          packet >> id >> x >> y >> hp >> maxHp >> mana >> maxMana >> gold >>
              recalling >> recallTimer;

          bool found = false;
          for (auto &entity : m_entities) {
            if (entity->getNetworkId() == id) {
              entity->setPosition({x, y});
              if (auto champ = dynamic_cast<Champion *>(entity.get())) {
                champ->setHealth(hp);
                champ->setMaxHealth(maxHp);
                champ->setMana(mana);
                champ->setMaxMana(maxMana);
                champ->setGold(gold);
                champ->setIsRecalling(recalling);
                champ->setRecallTimer(recallTimer);
              }
              found = true;
              break;
            }
          }

          // Si on ne connaît pas cette entité, on la crée (simplification
          // extrême : on crée des champions neutres pour les autres
          // joueurs/entités)
          if (!found) {
            auto newChamp = std::make_unique<Champion>(sf::Vector2f(x, y),
                                                       *m_gameMap, Team::ENEMY);
            newChamp->setNetworkId(id);
            newChamp->setHealth(hp);
            newChamp->setMaxHealth(maxHp);
            newChamp->setMana(mana);
            newChamp->setMaxMana(maxMana);
            newChamp->setGold(gold);
            newChamp->setIsRecalling(recalling);
            newChamp->setRecallTimer(recallTimer);
            m_entities.push_back(std::move(newChamp));
          }
        } else { // Generic CombatEntity (Nexus, Turret)
          uint32_t id;
          float x, y, hp, maxHp;
          packet >> id >> x >> y >> hp >> maxHp;

          for (auto &entity : m_entities) {
            if (entity->getNetworkId() == id) {
              if (auto combat = dynamic_cast<CombatEntity *>(entity.get())) {
                combat->setHealth(hp);
                combat->setMaxHealth(maxHp);
              }
              break;
            }
          }
        }
      }
      } else if (type == MessageType::ATTACK_ANIM) {
        uint32_t attackerId, targetId;
        if (packet >> attackerId >> targetId) {
          Champion* attacker = nullptr;
          CombatEntity* target = nullptr;
          for (auto& entity : m_entities) {
            if (entity->getNetworkId() == attackerId) attacker = dynamic_cast<Champion*>(entity.get());
            if (entity->getNetworkId() == targetId) target = dynamic_cast<CombatEntity*>(entity.get());
          }
          if (attacker && target) {
            attacker->playAttackAnimation(target);
          }
        }
      }
    }
  }
}

void Game::update(float deltaTime) {
  processNetwork();

  if (m_champion) {
    m_camera.setCenter(m_champion->getPosition());
  }

  // Seul l'indicateur de clic et potentiellement d'autres effets visuels ont
  // besoin de l'update() local
  for (auto &entity : m_entities) {
    if (dynamic_cast<ClickIndicator *>(entity.get())) {
      entity->update(deltaTime);
    }
    if (auto champ = dynamic_cast<Champion *>(entity.get())) {
      champ->updateVisuals(deltaTime);
    }
    // L'update des `Champion` locaux est désactivé car la position est
    // contrôlée par le réseau, mais on met à jour les visuels (lazers, etc.)
  }
}

void Game::render() {
  m_window.clear(sf::Color(30, 30, 30));

  m_renderer.renderWorld(m_window, m_camera, m_entities);
  m_renderer.renderMinimap(m_window, m_minimapView, m_entities);

  if (m_fontLoaded) {
    m_renderer.renderUI(m_window, m_hud.get(), m_uiWindows);
  }

  m_window.display();
}

void Game::sendPacket(sf::Packet &packet) {
  if (auto ipObj = sf::IpAddress::resolve(Config::Network::SERVER_IP)) {
    (void)m_socket.send(packet, ipObj.value(), Config::Network::SERVER_PORT);
  }
}

void Game::sendMove(float x, float y) {
  sf::Packet packet;
  packet << MessageType::MOVE << x << y;
  sendPacket(packet);
}

void Game::sendAttack(uint32_t targetId) {
  sf::Packet packet;
  packet << MessageType::ATTACK << targetId;
  sendPacket(packet);
}

void Game::sendSpell(std::uint8_t spellIndex) {
  sf::Packet packet;
  packet << MessageType::SPELL << spellIndex;
  sendPacket(packet);
}
