#include "HUD/LobbyUI.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

LobbyUI::LobbyUI(const sf::Font &font)
    : m_font(font), m_titleText(font, "Selection du Champion", 40),
      m_lockText(font, "Verrouiller", 24) {
  // Setup title
  m_titleText.setFillColor(sf::Color::White);
  m_titleText.setPosition({400.0f, 50.0f});

  // Setup lock button
  m_lockButton.setSize({200.0f, 60.0f});
  m_lockButton.setFillColor(sf::Color(100, 100, 100)); // Gris par defaut
  m_lockButton.setPosition({500.0f, 600.0f});

  m_lockText.setFillColor(sf::Color::White);
  m_lockText.setPosition({530.0f, 615.0f});

  loadChampionsData();
}

void LobbyUI::loadChampionsData() {
  std::string path = "assets/data/champions";
  float x = 200.0f;
  float y = 200.0f;

  for (const auto &entry : std::filesystem::directory_iterator(path)) {
    if (entry.path().extension() == ".json") {
      std::ifstream file(entry.path());
      if (file.is_open()) {
        nlohmann::json j;
        file >> j;

        ChampionData champ;
        champ.id = j.value("id", "inconnu");
        champ.name = j.value("name", "Inconnu");
        champ.texture_path = "assets/" + j.value("texture_path", "");

        champ.texture.emplace();
        if (champ.texture->loadFromFile(champ.texture_path)) {
          champ.sprite.emplace(champ.texture.value());
          // Mettre à l'echelle si besoin, supposons 64x64
          sf::FloatRect bounds = champ.sprite->getLocalBounds();
          champ.sprite->setScale(
              {100.0f / bounds.size.x, 100.0f / bounds.size.y});
          champ.sprite->setPosition({x, y});

          champ.frame.setSize({110.0f, 110.0f});
          champ.frame.setPosition({x - 5.0f, y - 5.0f});
          champ.frame.setFillColor(sf::Color::Transparent);
          champ.frame.setOutlineThickness(3.0f);
          champ.frame.setOutlineColor(sf::Color(50, 50, 50));

          m_champions.push_back(std::move(champ));
          x += 150.0f;
        }
      }
    }
  }
}

void LobbyUI::updateLobbyState(const std::vector<LobbyPlayerInfo> &players,
                               uint32_t myNetworkId) {
  m_players = players;
  m_myNetworkId = myNetworkId;

  // Verifier si moi j'ai verrouille
  for (const auto &p : m_players) {
    if (p.networkId == m_myNetworkId) {
      m_isLocked = p.isLocked;
      m_selectedId = p.selectedChampion;
      break;
    }
  }

  if (m_isLocked) {
    m_lockButton.setFillColor(sf::Color(50, 200, 50)); // Vert
    m_lockText.setString("Verrouille !");
  } else if (!m_selectedId.empty()) {
    m_lockButton.setFillColor(sf::Color(200, 150, 50)); // Orange (pret à lock)
  } else {
    m_lockButton.setFillColor(sf::Color(100, 100, 100)); // Gris
  }
}

void LobbyUI::handleEvent(const sf::Event &event,
                          const sf::RenderWindow &window) {
  if (m_isLocked)
    return;

  if (const auto *mouseButtonPressed =
          event.getIf<sf::Event::MouseButtonPressed>()) {
    if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
      sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
      sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);

      // Check lock button
      if (m_lockButton.getGlobalBounds().contains(worldPos)) {
        if (!m_selectedId.empty()) {
          m_hasPendingLock = true;
        }
      }

      // Check champions
      for (const auto &champ : m_champions) {
        if (champ.frame.getGlobalBounds().contains(worldPos)) {
          m_selectedId = champ.id;
          m_hasPendingSelection = true;
        }
      }
    }
  }
}

void LobbyUI::draw(sf::RenderWindow &window) {
  window.draw(m_titleText);

  // Dessin des champions
  for (auto &champ : m_champions) {
    if (champ.id == m_selectedId) {
      champ.frame.setOutlineColor(sf::Color::Yellow);
    } else {
      champ.frame.setOutlineColor(sf::Color(50, 50, 50));
    }

    // Est-ce qu'un autre joueur l'a pris ?
    bool taken = false;
    for (const auto &p : m_players) {
      if (p.networkId != m_myNetworkId && p.selectedChampion == champ.id) {
        taken = true;
        break;
      }
    }

    if (taken) {
      champ.frame.setOutlineColor(sf::Color::Red);
    }

    window.draw(champ.frame);
    if (champ.sprite.has_value()) {
      window.draw(champ.sprite.value());
    }

    sf::Text nameText(m_font, champ.name, 16);
    nameText.setFillColor(sf::Color::White);
    nameText.setPosition(
        {champ.frame.getPosition().x, champ.frame.getPosition().y + 115.0f});
    window.draw(nameText);
  }

  // Dessin du bouton lock
  window.draw(m_lockButton);
  window.draw(m_lockText);

  // Dessin de la liste des joueurs
  float py = 350.0f;
  for (const auto &p : m_players) {
    std::string status = p.isLocked ? " (Pret)" : " (Choix en cours...)";
    std::string champName =
        p.selectedChampion.empty() ? "?" : p.selectedChampion;

    sf::Text pText(m_font,
                   "Joueur " + std::to_string(p.networkId) + " : " + champName +
                       status,
                   20);
    pText.setFillColor(p.isLocked ? sf::Color::Green : sf::Color::White);
    pText.setPosition({200.0f, py});
    window.draw(pText);
    py += 30.0f;
  }
}
