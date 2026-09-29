#pragma once

#include <SFML/Network.hpp>
#include <vector>
#include <optional>

struct MatchmakingClient {
    std::optional<sf::IpAddress> ip;
    unsigned short port;
};

class MasterServer {
public:
    MasterServer();
    void run();

private:
    void processNetwork();
    void handleMatchmakingJoin(std::optional<sf::IpAddress> ip, unsigned short port);
    void launchGameInstance();

    sf::UdpSocket m_socket;
    std::vector<MatchmakingClient> m_queue;
    unsigned short m_nextInstancePort;
};
