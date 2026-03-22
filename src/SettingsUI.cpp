#include "SettingsUI.hpp"

SettingsUI::SettingsUI(const sf::Font& font)
    : UIWindow(font, "PARAMETRES", sf::Vector2f(500.0f, 400.0f), sf::Vector2f(200.0f, 150.0f),
               sf::Color(20, 20, 30, 245), sf::Color(80, 80, 120), sf::Color(50, 50, 70)),
      m_selectedCategory(SettingsCategory::VIDEO)
{}

UIAction SettingsUI::onHandleEvent(const sf::Event& event, const sf::Vector2f& mousePos) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f basePos = getBasePos();

        // Catégories (gauche)
        SettingsCategory catEnums[] = {SettingsCategory::VIDEO, SettingsCategory::AUDIO, SettingsCategory::GAME};
        for (int i = 0; i < 3; ++i) {
            sf::FloatRect catRect(basePos.x + 10, basePos.y + 50 + i * 45, 110, 35);
            if (catRect.contains(mousePos)) {
                m_selectedCategory = catEnums[i];
                return UIAction::CONSUMED;
            }
        }

        // Bouton Quitter la Partie
        sf::FloatRect quitBtn(basePos.x + 10, basePos.y + 350, 160, 35);
        if (quitBtn.contains(mousePos)) {
            return UIAction::QUIT;
        }
    }

    return UIAction::NONE;
}

void SettingsUI::onDraw(sf::RenderWindow& window, const sf::Vector2f& basePos) {
    const sf::Font& font = getFont();

    // Catégories (Gauche)
    sf::RectangleShape catBtn(sf::Vector2f(110.0f, 35.0f));
    sf::Text catText("", font, 14);
    std::string cats[] = {"Video", "Audio", "Jeu"};
    SettingsCategory catEnums[] = {SettingsCategory::VIDEO, SettingsCategory::AUDIO, SettingsCategory::GAME};

    for (int i = 0; i < 3; ++i) {
        catBtn.setPosition(basePos.x + 10, basePos.y + 50 + i * 45);
        catBtn.setFillColor(m_selectedCategory == catEnums[i] ? sf::Color(80, 80, 120) : sf::Color(40, 40, 55));
        catText.setString(cats[i]);
        catText.setPosition(basePos.x + 20, basePos.y + 57 + i * 45);
        window.draw(catBtn);
        window.draw(catText);
    }

    // Panneau de réglages (Droite)
    sf::RectangleShape panel(sf::Vector2f(350.0f, 280.0f));
    panel.setPosition(basePos.x + 135, basePos.y + 50);
    panel.setFillColor(sf::Color(30, 30, 40, 200));
    panel.setOutlineThickness(1.0f);
    panel.setOutlineColor(sf::Color(60, 60, 80));
    window.draw(panel);

    sf::Text optionTitle("", font, 16);
    optionTitle.setFillColor(sf::Color(200, 200, 240));
    optionTitle.setPosition(basePos.x + 150, basePos.y + 60);

    sf::Text optionDesc("", font, 13);
    optionDesc.setFillColor(sf::Color(160, 160, 180));

    if (m_selectedCategory == SettingsCategory::VIDEO) {
        optionTitle.setString("Parametres Video");
        window.draw(optionTitle);
        optionDesc.setString("Resolution : Plein ecran");
        optionDesc.setPosition(basePos.x + 150, basePos.y + 100); window.draw(optionDesc);
        optionDesc.setString("VSync : Active");
        optionDesc.setPosition(basePos.x + 150, basePos.y + 130); window.draw(optionDesc);
        optionDesc.setString("FPS Max : 60");
        optionDesc.setPosition(basePos.x + 150, basePos.y + 160); window.draw(optionDesc);
    } else if (m_selectedCategory == SettingsCategory::AUDIO) {
        optionTitle.setString("Parametres Audio");
        window.draw(optionTitle);
        optionDesc.setString("Volume General : 100%");
        optionDesc.setPosition(basePos.x + 150, basePos.y + 100); window.draw(optionDesc);
        optionDesc.setString("Volume Musique : 80%");
        optionDesc.setPosition(basePos.x + 150, basePos.y + 130); window.draw(optionDesc);
        optionDesc.setString("Volume Effets : 100%");
        optionDesc.setPosition(basePos.x + 150, basePos.y + 160); window.draw(optionDesc);
    } else if (m_selectedCategory == SettingsCategory::GAME) {
        optionTitle.setString("Parametres de Jeu");
        window.draw(optionTitle);
        optionDesc.setString("Langue : Francais");
        optionDesc.setPosition(basePos.x + 150, basePos.y + 100); window.draw(optionDesc);
        optionDesc.setString("Afficher Minimap : Oui");
        optionDesc.setPosition(basePos.x + 150, basePos.y + 130); window.draw(optionDesc);
        optionDesc.setString("Afficher Chemin Debug : Oui");
        optionDesc.setPosition(basePos.x + 150, basePos.y + 160); window.draw(optionDesc);
    }

    // Bouton Quitter la Partie (Bas-Gauche)
    sf::RectangleShape quitBtn(sf::Vector2f(160.0f, 35.0f));
    quitBtn.setPosition(basePos.x + 10, basePos.y + 350);
    quitBtn.setFillColor(sf::Color(180, 40, 40));
    quitBtn.setOutlineThickness(1.0f);
    quitBtn.setOutlineColor(sf::Color(220, 80, 80));
    sf::Text quitText("QUITTER LA PARTIE", font, 12);
    quitText.setFillColor(sf::Color::White);
    quitText.setPosition(basePos.x + 18, basePos.y + 358);
    window.draw(quitBtn);
    window.draw(quitText);
}
