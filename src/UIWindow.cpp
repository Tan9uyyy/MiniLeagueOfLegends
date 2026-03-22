#include "UIWindow.hpp"

UIWindow::UIWindow(const sf::Font& font, const std::string& title,
                   sf::Vector2f size, sf::Vector2f position,
                   sf::Color bgColor, sf::Color outlineColor, sf::Color titleBarColor)
    : m_font(font), m_titleText(font, title, 14), m_isOpen(false), m_isDragging(false)
{
    m_background.setSize(size);
    m_background.setFillColor(bgColor);
    m_background.setOutlineThickness(2.0f);
    m_background.setOutlineColor(outlineColor);
    m_background.setPosition(position);

    m_titleBar.setSize(sf::Vector2f(size.x, 30.0f));
    m_titleBar.setFillColor(titleBarColor);
    m_titleBar.setPosition(position);

    m_titleText = sf::Text(m_font, title, 14);
    m_titleText.setFillColor(sf::Color::White);
    m_titleText.setPosition({position.x + 10.0f, position.y + 5.0f});
}

bool UIWindow::containsMouse(const sf::Vector2f& mousePos) const {
    return m_background.getGlobalBounds().contains(mousePos);
}

UIAction UIWindow::handleEvent(const sf::Event& event, const sf::Vector2i& pixelPos, const sf::RenderWindow& window) {
    if (!m_isOpen) return UIAction::NONE;

    sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos, window.getDefaultView());

    // --- Drag & Drop (commun à toutes les fenêtres) ---
    if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mousePressed->button == sf::Mouse::Button::Left) {
            if (m_titleBar.getGlobalBounds().contains(mousePos)) {
                m_isDragging = true;
                m_dragOffset = m_background.getPosition() - mousePos;
                return UIAction::CONSUMED;
            }
        }
    }

    if (const auto* mouseReleased = event.getIf<sf::Event::MouseButtonReleased>()) {
        if (mouseReleased->button == sf::Mouse::Button::Left) {
            m_isDragging = false;
        }
    }

    if (event.is<sf::Event::MouseMoved>() && m_isDragging) {
        sf::Vector2f newPos = mousePos + m_dragOffset;
        m_background.setPosition(newPos);
        m_titleBar.setPosition(newPos);
        m_titleText.setPosition({newPos.x + 10.0f, newPos.y + 5.0f});
        return UIAction::CONSUMED;
    }

    // --- Déléguer à la classe fille ---
    UIAction childAction = onHandleEvent(event, mousePos);
    if (childAction != UIAction::NONE) return childAction;

    // --- Blocage générique : tout clic dans la fenêtre est consommé ---
    if (event.is<sf::Event::MouseButtonPressed>() || event.is<sf::Event::MouseButtonReleased>()) {
        if (m_background.getGlobalBounds().contains(mousePos)) return UIAction::CONSUMED;
    }

    return UIAction::NONE;
}

void UIWindow::draw(sf::RenderWindow& window) {
    if (!m_isOpen) return;

    window.setView(window.getDefaultView());

    // Fond + Barre de titre (commun)
    window.draw(m_background);
    window.draw(m_titleBar);
    window.draw(m_titleText);

    // Contenu spécifique
    onDraw(window, m_background.getPosition());
}
