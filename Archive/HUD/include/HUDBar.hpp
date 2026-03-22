#pragma once
#include "HUDComponent.hpp"

class BarComponent : public HUDComponent {
public:
    BarComponent(Champion* champion, const sf::Font& font, float width, float height, sf::Color color);
    void draw(sf::RenderWindow& window) override;

protected:
    float m_width, m_height;
    sf::Color m_color;
    virtual float getPercentage() const = 0;
    virtual std::string getText() const { return ""; }
};

class HealthBar : public BarComponent {
public:
    HealthBar(Champion* champion, const sf::Font& font);
protected:
    float getPercentage() const override;
    std::string getText() const override;
};

class ManaBar : public BarComponent {
public:
    ManaBar(Champion* champion, const sf::Font& font);
protected:
    float getPercentage() const override;
    std::string getText() const override;
};

class XPBar : public BarComponent {
public:
    XPBar(Champion* champion, const sf::Font& font);
    void draw(sf::RenderWindow& window) override; // Vertical fill logic
protected:
    float getPercentage() const override;
};
