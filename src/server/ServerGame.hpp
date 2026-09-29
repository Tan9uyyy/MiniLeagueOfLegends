#pragma once

#include <SFML/Network.hpp>
#include <vector>
#include <memory>
#include "Entity.hpp"
#include "Map.hpp"
#include "Champion.hpp"

#include <optional>

enum class ServerState {
    LOBBY,
    PLAYING
};

struct ClientInfo {
    std::optional<sf::IpAddress> ip;
    unsigned short port;
    uint32_t championId;
    std::string selectedChampion = "";
    bool isLocked = false;
};

class ServerGame {
public:
    ServerGame(unsigned short port);
    void run();

private:
    void initWorld();
    void processNetwork();
    void update(float deltaTime);
    void broadcastState();
    void checkLobbyStatus();
    void broadcastLobbyState();

    sf::UdpSocket m_socket;
    ServerState m_serverState = ServerState::LOBBY;
    
    Map* m_gameMap;
    std::vector<std::unique_ptr<Entity>> m_entities;
    std::vector<ClientInfo> m_clients;
    
    uint32_t m_nextNetworkId = 1;
};
