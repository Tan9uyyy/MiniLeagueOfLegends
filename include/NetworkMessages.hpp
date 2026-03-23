#pragma once
#include <SFML/Network.hpp>
#include <cstdint>

// Types de messages UDP pour notre architecture Client/Serveur
enum class MessageType : std::uint8_t {
    JOIN,        // Client demande à rejoindre le jeu
    JOIN_ACK,    // Serveur accepte et envoie le NetworkID du champion
    MOVE,        // Client ordonne un déplacement (clic droit)
    ATTACK,      // Client attaque une entité
    SPELL,       // Client lance un sort
    STATE        // Serveur envoie les positions de toutes les entités
};

// Surcharge pour sf::Packet avec notre énumération
inline sf::Packet& operator<<(sf::Packet& packet, MessageType type) {
    return packet << static_cast<std::uint8_t>(type);
}

inline sf::Packet& operator>>(sf::Packet& packet, MessageType& type) {
    std::uint8_t byte;
    packet >> byte;
    type = static_cast<MessageType>(byte);
    return packet;
}
