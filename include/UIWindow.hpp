#pragma once
#include <SFML/Graphics.hpp>

// Code de retour des événements UI
enum class UIAction {
    NONE,       // Événement non consommé
    CONSUMED,   // Événement consommé par la fenêtre
    QUIT        // L'utilisateur veut quitter la partie
};

// Classe de base abstraite pour toutes les fenêtres déplaçables (Boutique, Paramètres, etc.)
class UIWindow {
public:
    UIWindow(const sf::Font& font, const std::string& title,
             sf::Vector2f size, sf::Vector2f position,
             sf::Color bgColor = sf::Color(20, 20, 20, 240),
             sf::Color outlineColor = sf::Color(100, 100, 100),
             sf::Color titleBarColor = sf::Color(40, 40, 40));

    virtual ~UIWindow() = default;

    void toggle() { m_isOpen = !m_isOpen; }
    bool isOpen() const { return m_isOpen; }

    // Gère un événement souris. Retourne UIAction indiquant si l'événement a été consommé.
    UIAction handleEvent(const sf::Event& event, const sf::Vector2i& pixelPos, const sf::RenderWindow& window);

    // Dessine la fenêtre complète (fond + contenu spécifique)
    void draw(sf::RenderWindow& window);

    // Vérifie si un point (en coordonnées UI) est dans la fenêtre
    bool containsMouse(const sf::Vector2f& mousePos) const;

protected:
    // À implémenter par les classes filles
    virtual UIAction onHandleEvent(const sf::Event& event, const sf::Vector2f& mousePos) = 0;
    virtual void onDraw(sf::RenderWindow& window, const sf::Vector2f& basePos) = 0;

    // Accesseurs protégés pour les classes filles
    const sf::Font& getFont() const { return m_font; }
    sf::Vector2f getBasePos() const { return m_background.getPosition(); }

private:
    const sf::Font& m_font;
    bool m_isOpen;

    // Éléments UI communs
    sf::RectangleShape m_background;
    sf::RectangleShape m_titleBar;
    sf::Text m_titleText;

    // Drag & Drop
    bool m_isDragging;
    sf::Vector2f m_dragOffset;
};
