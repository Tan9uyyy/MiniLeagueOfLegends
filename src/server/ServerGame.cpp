#include "ServerGame.hpp"
#include "Config.hpp"
#include "Nexus.hpp"
#include "Turret.hpp"
#include "NetworkMessages.hpp"
#include "NetworkIds.hpp"
#include <iostream>

ServerGame::ServerGame(unsigned short port) {
    if (m_socket.bind(port) != sf::Socket::Status::Done) {
        std::cerr << "Serveur : Erreur lors du bind sur le port " << port << std::endl;
        std::exit(EXIT_FAILURE);
    }
    m_socket.setBlocking(false);
    initWorld();
    std::cout << "Serveur demarre sur le port " << port << std::endl;
}

void ServerGame::initWorld() {
    auto mapPtr = std::make_unique<Map>(Config::Game::MAP_WIDTH, Config::Game::MAP_HEIGHT);
    mapPtr->setNetworkId(NetworkIds::MAP);
    m_gameMap = mapPtr.get();

    // Nexus Allié
    auto alliedNexus = std::make_unique<Nexus>(Config::Map::ALLIED_NEXUS_POS, Team::ALLIED);
    m_gameMap->addObstacle(alliedNexus->getBounds());
    alliedNexus->setNetworkId(NetworkIds::ALLIED_NEXUS);
    m_entities.push_back(std::move(alliedNexus));

    // Tourelles Alliées
    auto turretA1 = std::make_unique<Turret>(Config::Map::ALLIED_TURRET_1_POS, Team::ALLIED);
    auto turretA2 = std::make_unique<Turret>(Config::Map::ALLIED_TURRET_2_POS, Team::ALLIED);
    m_gameMap->addObstacle(turretA1->getBounds());
    m_gameMap->addObstacle(turretA2->getBounds());
    turretA1->setNetworkId(NetworkIds::ALLIED_TURRET_1);
    turretA2->setNetworkId(NetworkIds::ALLIED_TURRET_2);
    m_entities.push_back(std::move(turretA1));
    m_entities.push_back(std::move(turretA2));

    // Nexus Ennemi
    auto enemyNexus = std::make_unique<Nexus>(Config::Map::ENEMY_NEXUS_POS, Team::ENEMY);
    m_gameMap->addObstacle(enemyNexus->getBounds());
    enemyNexus->setNetworkId(NetworkIds::ENEMY_NEXUS);
    m_entities.push_back(std::move(enemyNexus));

    // Tourelles Ennemies
    auto turretE1 = std::make_unique<Turret>(Config::Map::ENEMY_TURRET_1_POS, Team::ENEMY);
    auto turretE2 = std::make_unique<Turret>(Config::Map::ENEMY_TURRET_2_POS, Team::ENEMY);
    m_gameMap->addObstacle(turretE1->getBounds());
    m_gameMap->addObstacle(turretE2->getBounds());
    turretE1->setNetworkId(NetworkIds::ENEMY_TURRET_1);
    turretE2->setNetworkId(NetworkIds::ENEMY_TURRET_2);
    m_entities.push_back(std::move(turretE1));
    m_entities.push_back(std::move(turretE2));

    // Map ajoutée en dernier (pour l'ordre de rendu côté client si on envoie tous les IDs de la même façon)
    m_entities.push_back(std::move(mapPtr));
    m_nextNetworkId = NetworkIds::START_DYNAMIC_ID;
}

void ServerGame::processNetwork() {
    sf::Packet packet;
    std::optional<sf::IpAddress> senderIp;
    unsigned short senderPort;

    while (m_socket.receive(packet, senderIp, senderPort) == sf::Socket::Status::Done && senderIp.has_value()) {
        MessageType type;
        if (packet >> type) {
            if (type == MessageType::JOIN) {
                std::cout << "Serveur : Nouveau client " << senderIp.value() << ":" << senderPort << std::endl;
                
                uint32_t champId = m_nextNetworkId++;
                
                ClientInfo newClient;
                newClient.ip = senderIp.value();
                newClient.port = senderPort;
                newClient.championId = champId;
                m_clients.push_back(newClient);

                // Répondre avec l'ID du champion
                sf::Packet reply;
                reply << MessageType::JOIN_ACK << champId;
                (void)m_socket.send(reply, senderIp.value(), senderPort);

            } else if (type == MessageType::LOBBY_SELECT_CHAMPION && m_serverState == ServerState::LOBBY) {
                std::string championId;
                if (packet >> championId) {
                    for (auto& client : m_clients) {
                        if (client.ip.has_value() && client.ip.value() == senderIp.value() && client.port == senderPort) {
                            if (!client.isLocked) {
                                client.selectedChampion = championId;
                            }
                            break;
                        }
                    }
                }
            } else if (type == MessageType::LOBBY_LOCK_CHAMPION && m_serverState == ServerState::LOBBY) {
                for (auto& client : m_clients) {
                    if (client.ip.has_value() && client.ip.value() == senderIp.value() && client.port == senderPort) {
                        if (!client.selectedChampion.empty()) {
                            client.isLocked = true;
                        }
                        break;
                    }
                }
                checkLobbyStatus();
            } else if (type == MessageType::MOVE && m_serverState == ServerState::PLAYING) {
                float targetX, targetY;
                if (packet >> targetX >> targetY) {
                    for (const auto& client : m_clients) {
                        if (client.ip.has_value() && client.ip.value() == senderIp.value() && client.port == senderPort) {
                            for (auto& entity : m_entities) {
                                if (entity->getNetworkId() == client.championId) {
                                    entity->setTargetPosition({targetX, targetY});
                                    break;
                                }
                            }
                            break;
                        }
                    }
                }
            } else if (type == MessageType::ATTACK && m_serverState == ServerState::PLAYING) {
                uint32_t targetId;
                if (packet >> targetId) {
                    for (const auto& client : m_clients) {
                        if (client.ip.has_value() && client.ip.value() == senderIp.value() && client.port == senderPort) {
                            Champion* playerChamp = nullptr;
                            CombatEntity* targetEntity = nullptr;
                            for (auto& entity : m_entities) {
                                if (entity->getNetworkId() == client.championId) {
                                    playerChamp = dynamic_cast<Champion*>(entity.get());
                                }
                                if (entity->getNetworkId() == targetId) {
                                    targetEntity = dynamic_cast<CombatEntity*>(entity.get());
                                }
                            }
                            if (playerChamp && targetEntity) {
                                playerChamp->setTargetEntity(targetEntity);
                            }
                            break;
                        }
                    }
                }
            } else if (type == MessageType::SPELL && m_serverState == ServerState::PLAYING) {
                uint8_t spellIndex;
                if (packet >> spellIndex) {
                    for (const auto& client : m_clients) {
                        if (client.ip.has_value() && client.ip.value() == senderIp.value() && client.port == senderPort) {
                            for (auto& entity : m_entities) {
                                if (entity->getNetworkId() == client.championId) {
                                    if (auto champ = dynamic_cast<Champion*>(entity.get())) {
                                        if (spellIndex == 4) {
                                            champ->startRecall();
                                        } else {
                                            champ->castSpell(spellIndex);
                                        }
                                    }
                                    break;
                                }
                            }
                            break;
                        }
                    }
                }
            } else if (type == MessageType::BUY_ITEM && m_serverState == ServerState::PLAYING) {
                int itemId;
                if (packet >> itemId) {
                    for (const auto& client : m_clients) {
                        if (client.ip.has_value() && client.ip.value() == senderIp.value() && client.port == senderPort) {
                            for (auto& entity : m_entities) {
                                if (entity->getNetworkId() == client.championId) {
                                    if (auto champ = dynamic_cast<Champion*>(entity.get())) {
                                        if (champ->canShop()) {
                                            for (const auto& item : ItemTemplate::getShopItems()) {
                                                if (item.id == itemId) { champ->buyItem(item); break; }
                                            }
                                        }
                                    }
                                    break;
                                }
                            }
                            break;
                        }
                    }
                }
            } else if (type == MessageType::SELL_ITEM && m_serverState == ServerState::PLAYING) {
                int inventoryIndex;
                if (packet >> inventoryIndex) {
                    for (const auto& client : m_clients) {
                        if (client.ip.has_value() && client.ip.value() == senderIp.value() && client.port == senderPort) {
                            for (auto& entity : m_entities) {
                                if (entity->getNetworkId() == client.championId) {
                                    if (auto champ = dynamic_cast<Champion*>(entity.get())) {
                                        if (champ->canShop()) {
                                            champ->sellItem(inventoryIndex);
                                        }
                                    }
                                    break;
                                }
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
}

void ServerGame::update(float deltaTime) {
    for (auto& entity : m_entities) {
        entity->update(deltaTime);

        if (auto champ = dynamic_cast<Champion*>(entity.get())) {
            // Regeneration Base (10% HP/Mana par seconde dans la fontaine)
            if (m_gameMap->isInSpawnArea(champ->getPosition(), champ->getTeam())) {
                champ->regen(0.10f, 0.10f, deltaTime);
            }

            uint32_t targetId;
            if (champ->popJustAttacked(targetId)) {
                sf::Packet p;
                p << MessageType::ATTACK_ANIM << champ->getNetworkId() << targetId;
                for (const auto& client : m_clients) {
                    if (client.ip.has_value()) {
                        (void)m_socket.send(p, client.ip.value(), client.port);
                    }
                }
            }
        }
    }
}

void ServerGame::broadcastState() {
    if (m_clients.empty()) return;

    sf::Packet statePacket;
    // On compte combien d'entités on va envoyer
    uint32_t validEntitiesCount = 0;
    for (const auto& entity : m_entities) {
        if (dynamic_cast<Map*>(entity.get())) continue;
        validEntitiesCount++;
    }

    statePacket << MessageType::STATE << validEntitiesCount;
    for (const auto& entity : m_entities) {
        if (dynamic_cast<Map*>(entity.get())) continue;

        if (auto champion = dynamic_cast<Champion*>(entity.get())) {
            sf::Vector2f pos = champion->getPosition();
            statePacket << (uint8_t)1 // 1 = Champion
                        << champion->getNetworkId() << pos.x << pos.y
                        << champion->getHealth() << champion->getMaxHealth()
                        << champion->getMana() << champion->getMaxMana()
                        << champion->getGold() 
                        << champion->isRecalling() << champion->getRecallTimer()
                        << champion->getRespawnTimer();
        } else if (auto combat = dynamic_cast<CombatEntity*>(entity.get())) {
            sf::Vector2f pos = entity->getPosition(); // Will be 0,0 for Buildings but it's ok
            statePacket << (uint8_t)0 // 0 = Generic CombatEntity
                        << entity->getNetworkId() << pos.x << pos.y
                        << combat->getHealth() << combat->getMaxHealth();
        }
    }

    for (const auto& client : m_clients) {
        if (client.ip.has_value()) {
            (void)m_socket.send(statePacket, client.ip.value(), client.port);
        }
    }
}

void ServerGame::checkLobbyStatus() {
    if (m_serverState != ServerState::LOBBY) return;
    if (m_clients.size() < (size_t)Config::Network::PLAYERS_PER_MATCH) return;

    bool allLocked = true;
    for (const auto& client : m_clients) {
        if (!client.isLocked) {
            allLocked = false;
            break;
        }
    }

    if (allLocked) {
        std::cout << "Tous les joueurs sont prets. Lancement de la partie !" << std::endl;
        
        // Spawn champions
        for (const auto& client : m_clients) {
            sf::Vector2f spawnPos = m_gameMap->getSpawnPosition(Team::ALLIED);
            std::cout << "Spawn champion: " << client.selectedChampion << " pour l'ID " << client.championId << std::endl;
            auto champion = std::make_unique<Champion>(spawnPos, *m_gameMap, Team::ALLIED);
            champion->setNetworkId(client.championId);
            m_entities.push_back(std::move(champion));
        }

        m_serverState = ServerState::PLAYING;

        sf::Packet notify;
        notify << MessageType::LOBBY_START_GAME;
        for (const auto& client : m_clients) {
            if (client.ip.has_value()) {
                (void)m_socket.send(notify, client.ip.value(), client.port);
            }
        }
    }
}

void ServerGame::broadcastLobbyState() {
    if (m_clients.empty()) return;

    sf::Packet statePacket;
    statePacket << MessageType::LOBBY_STATE;
    statePacket << static_cast<uint32_t>(m_clients.size());

    for (const auto& client : m_clients) {
        statePacket << client.championId << client.selectedChampion << client.isLocked;
    }

    for (const auto& client : m_clients) {
        if (client.ip.has_value()) {
            (void)m_socket.send(statePacket, client.ip.value(), client.port);
        }
    }
}

void ServerGame::run() {
    sf::Clock clock;
    sf::Clock networkClock;
    const float NETWORK_TICK_RATE = Config::Network::TICK_RATE;

    while (true) {
        float deltaTime = clock.restart().asSeconds();
        
        processNetwork();
        
        if (m_serverState == ServerState::PLAYING) {
            update(deltaTime);
        }
        
        if (networkClock.getElapsedTime().asSeconds() >= NETWORK_TICK_RATE) {
            if (m_serverState == ServerState::LOBBY) {
                broadcastLobbyState();
            } else {
                broadcastState();
            }
            networkClock.restart();
        }

        sf::sleep(sf::milliseconds(1)); // Limite CPU
    }
}
