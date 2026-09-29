#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

// We don't include json.hpp here to speed up compilation, only in cpp

#include <optional>

struct ChampionData {
    std::string id;
    std::string name;
    std::string texture_path;
    std::optional<sf::Texture> texture;
    std::optional<sf::Sprite> sprite;
    sf::RectangleShape frame; // Pour cliquer et dessiner le bouton
};

struct LobbyPlayerInfo {
    uint32_t networkId;
    std::string selectedChampion;
    bool isLocked;
};

class LobbyUI {
public:
    LobbyUI(const sf::Font& font);
    ~LobbyUI() = default;

    void updateLobbyState(const std::vector<LobbyPlayerInfo>& players, uint32_t myNetworkId);
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);

    bool hasPendingSelection() const { return m_hasPendingSelection; }
    std::string popSelection() { m_hasPendingSelection = false; return m_selectedId; }

    bool hasPendingLock() const { return m_hasPendingLock; }
    void popLock() { m_hasPendingLock = false; }

private:
    void loadChampionsData();
    
    sf::Font m_font;
    std::vector<ChampionData> m_champions;
    std::vector<LobbyPlayerInfo> m_players;
    uint32_t m_myNetworkId = 0;

    std::string m_selectedId = "";
    bool m_hasPendingSelection = false;
    
    bool m_isLocked = false;
    bool m_hasPendingLock = false;

    // UI Elements
    sf::RectangleShape m_lockButton;
    sf::Text m_lockText;
    sf::Text m_titleText;
};
