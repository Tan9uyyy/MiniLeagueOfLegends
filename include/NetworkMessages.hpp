#pragma once
#include <SFML/Network.hpp>
#include <cstdint>

// Types de messages UDP pour notre architecture Client/Serveur
enum class MessageType : std::uint8_t {
    JOIN,        // Client demande à rejoindre le jeu
    JOIN_ACK,    // Serveur accepte et envoie le NetworkID du champion
    MATCHMAKING_JOIN,      // Client demande à rejoindre la file
    MATCHMAKING_QUEUE_ACK, // Serveur maitre confirme la mise en file
    MATCHMAKING_FOUND,     // Serveur maitre notifie que la partie est prête (avec le port)
    LOBBY_STATE,           // Etat du lobby (Serveur -> Client)
    LOBBY_SELECT_CHAMPION, // Choix de champion (Client -> Serveur)
    LOBBY_LOCK_CHAMPION,   // Verrouillage du choix (Client -> Serveur)
    LOBBY_START_GAME,      // Lancement de la partie (Serveur -> Client)
    MOVE,        // Client ordonne un déplacement (clic droit)
    ATTACK,      // Client attaque une entité
    SPELL,       // Client lance un sort
    STATE,       // Serveur envoie les positions de toutes les entités
    ATTACK_ANIM, // Serveur signale qu'une attaque a eu lieu (pour jouer l'animation)
    BUY_ITEM,    // Client achète un objet
    SELL_ITEM    // Client vend un objet
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
