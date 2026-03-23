#pragma once

#include <SFML/Network.hpp>
#include <vector>
#include <memory>
#include "Entity.hpp"
#include "Map.hpp"
#include "Champion.hpp"

#include <optional>

struct ClientInfo {
    std::optional<sf::IpAddress> ip;
    unsigned short port;
    uint32_t championId;
};

class ServerGame {
public:
    ServerGame();
    void run();

private:
    void initWorld();
    void processNetwork();
    void update(float deltaTime);
    void broadcastState();

    sf::UdpSocket m_socket;
    
    Map* m_gameMap;
    std::vector<std::unique_ptr<Entity>> m_entities;
    std::vector<ClientInfo> m_clients;
    
    uint32_t m_nextNetworkId = 1;
};
