#include "ServerGame.hpp"
#include "Config.hpp"
#include <cstdlib>

int main(int argc, char** argv) {
    unsigned short port = Config::Network::SERVER_PORT;
    if (argc > 1) {
        port = static_cast<unsigned short>(std::atoi(argv[1]));
    }
    ServerGame server(port);
    server.run();
    return 0;
}
