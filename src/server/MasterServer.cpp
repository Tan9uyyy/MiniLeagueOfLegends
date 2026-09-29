#include "MasterServer.hpp"
#include "Config.hpp"
#include "NetworkMessages.hpp"
#include <cstdlib>
#include <iostream>
#include <string>

MasterServer::MasterServer()
    : m_nextInstancePort(54322) { // Start allocating from 54322
  if (m_socket.bind(Config::Network::MASTER_SERVER_PORT) !=
      sf::Socket::Status::Done) {
    std::cerr << "Erreur : Impossible de lier le socket sur le port "
              << Config::Network::MASTER_SERVER_PORT << std::endl;
  }
  m_socket.setBlocking(false);
  std::cout << "Master Server demarre (Port "
            << Config::Network::MASTER_SERVER_PORT << ")..." << std::endl;
}

void MasterServer::run() {
  std::cout << "En attente de joueurs (" << Config::Network::PLAYERS_PER_MATCH
            << " requis par partie)..." << std::endl;

  while (true) {
    processNetwork();
    sf::sleep(sf::milliseconds(5)); // Limiter la charge CPU
  }
}

void MasterServer::processNetwork() {
  sf::Packet packet;
  std::optional<sf::IpAddress> senderIp;
  unsigned short senderPort;

  while (m_socket.receive(packet, senderIp, senderPort) ==
             sf::Socket::Status::Done &&
         senderIp.has_value()) {
    MessageType type;
    if (packet >> type) {
      if (type == MessageType::MATCHMAKING_JOIN) {
        handleMatchmakingJoin(senderIp, senderPort);
      }
    }
  }
}

void MasterServer::handleMatchmakingJoin(std::optional<sf::IpAddress> ip,
                                         unsigned short port) {
  if (!ip.has_value())
    return;

  // Verifier si le client est deja dans la file
  for (const auto &client : m_queue) {
    if (client.ip.has_value() && client.ip.value() == ip.value() &&
        client.port == port) {
      return; // Client deja dans la file
    }
  }

  m_queue.push_back({ip, port});
  std::cout << "Vient de rejoindre le matchmaking : " << ip.value() << ":"
            << port << " (" << m_queue.size() << "/"
            << Config::Network::PLAYERS_PER_MATCH << ")" << std::endl;

  // Acknowledgement optionnel, on pourrait afficher un texte en attente sur
  // l'interface
  sf::Packet ack;
  ack << MessageType::MATCHMAKING_QUEUE_ACK;
  (void)m_socket.send(ack, ip.value(), port);

  // Verifier si on a assez de joueurs
  if (m_queue.size() >= Config::Network::PLAYERS_PER_MATCH) {
    launchGameInstance();
  }
}

void MasterServer::launchGameInstance() {
  unsigned short port = m_nextInstancePort++;

  std::cout << "Creation d'une instance de jeu sur le port " << port << "..."
            << std::endl;

  // Lancer le processus du serveur de jeu
  // Attention: La syntaxe differe selon l'OS. Sous Linux "&" permet de
  // détacher, ou "nohup ./MiniLeagueOfLegendsServer PORT &"
  std::string cmd = "./build-linux/bin/MiniLeagueOfLegendsServer " +
                    std::to_string(port) + " &";
  int result = std::system(cmd.c_str());
  if (result != 0) {
    std::cerr << "Erreur lors du lancement de l'instance (" << cmd << ")"
              << std::endl;
    // On ne notifie pas les clients si echec
    return;
  }

  // Avertir les clients dans la file
  sf::Packet notify;
  notify << MessageType::MATCHMAKING_FOUND << port;
  for (const auto &client : m_queue) {
    if (client.ip.has_value()) {
      (void)m_socket.send(notify, client.ip.value(), client.port);
    }
  }

  // Vider la file
  m_queue.clear();
}
