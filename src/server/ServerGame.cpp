#include "ServerGame.hpp"
#include "Nexus.hpp"
#include "Turret.hpp"
#include "NetworkMessages.hpp"
#include "NetworkIds.hpp"
#include <iostream>

ServerGame::ServerGame() {
    if (m_socket.bind(54321) != sf::Socket::Status::Done) {
        std::cerr << "Serveur : Erreur lors du bind sur le port 54321" << std::endl;
    }
    m_socket.setBlocking(false);
    initWorld();
    std::cout << "Serveur demarre sur le port 54321" << std::endl;
}

void ServerGame::initWorld() {
    auto mapPtr = std::make_unique<Map>(10000.0f, 10000.0f);
    mapPtr->setNetworkId(NetworkIds::MAP);
    m_gameMap = mapPtr.get();

    // Nexus Allié
    auto alliedNexus = std::make_unique<Nexus>(sf::Vector2f(1000.0f, 9000.0f), Team::ALLIED);
    m_gameMap->addObstacle(alliedNexus->getBounds());
    alliedNexus->setNetworkId(NetworkIds::ALLIED_NEXUS);
    m_entities.push_back(std::move(alliedNexus));

    // Tourelles Alliées
    auto turretA1 = std::make_unique<Turret>(sf::Vector2f(1200.0f, 8500.0f), Team::ALLIED);
    auto turretA2 = std::make_unique<Turret>(sf::Vector2f(1500.0f, 8800.0f), Team::ALLIED);
    m_gameMap->addObstacle(turretA1->getBounds());
    m_gameMap->addObstacle(turretA2->getBounds());
    turretA1->setNetworkId(NetworkIds::ALLIED_TURRET_1);
    turretA2->setNetworkId(NetworkIds::ALLIED_TURRET_2);
    m_entities.push_back(std::move(turretA1));
    m_entities.push_back(std::move(turretA2));

    // Nexus Ennemi
    auto enemyNexus = std::make_unique<Nexus>(sf::Vector2f(2500.0f, 8000.0f), Team::ENEMY);
    m_gameMap->addObstacle(enemyNexus->getBounds());
    enemyNexus->setNetworkId(NetworkIds::ENEMY_NEXUS);
    m_entities.push_back(std::move(enemyNexus));

    // Tourelles Ennemies
    auto turretE1 = std::make_unique<Turret>(sf::Vector2f(2200.0f, 8500.0f), Team::ENEMY);
    auto turretE2 = std::make_unique<Turret>(sf::Vector2f(2500.0f, 8300.0f), Team::ENEMY);
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
                
                // Créer un champion pour ce joueur
                // On les met par défaut dans l'équipe bleue, mais on pourrait alterner
                sf::Vector2f spawnPos = m_gameMap->getSpawnPosition(Team::ALLIED); 
                auto champion = std::make_unique<Champion>(spawnPos, *m_gameMap, Team::ALLIED);
                
                uint32_t champId = m_nextNetworkId++;
                champion->setNetworkId(champId);
                m_entities.push_back(std::move(champion));
                
                ClientInfo newClient;
                newClient.ip = senderIp.value();
                newClient.port = senderPort;
                newClient.championId = champId;
                m_clients.push_back(newClient);

                // Répondre avec l'ID du champion
                sf::Packet reply;
                reply << MessageType::JOIN_ACK << champId;
                (void)m_socket.send(reply, senderIp.value(), senderPort);

            } else if (type == MessageType::MOVE) {
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
            } else if (type == MessageType::ATTACK) {
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
            } else if (type == MessageType::SPELL) {
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
            }
        }
    }
}

void ServerGame::update(float deltaTime) {
    for (auto& entity : m_entities) {
        entity->update(deltaTime);
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
                        << champion->isRecalling() << champion->getRecallTimer();
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

void ServerGame::run() {
    sf::Clock clock;
    sf::Clock networkClock;
    const float NETWORK_TICK_RATE = 1.0f / 30.0f; // 30 paquets par seconde

    while (true) {
        float deltaTime = clock.restart().asSeconds();
        
        processNetwork();
        update(deltaTime);
        
        if (networkClock.getElapsedTime().asSeconds() >= NETWORK_TICK_RATE) {
            broadcastState();
            networkClock.restart();
        }

        sf::sleep(sf::milliseconds(1)); // Limite CPU
    }
}
