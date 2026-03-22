#pragma once
#include "UIWindow.hpp"
#include "Champion.hpp"
#include "Item.hpp"

class ShopUI : public UIWindow {
public:
    ShopUI(Champion* champion, const sf::Font& font);

protected:
    UIAction onHandleEvent(const sf::Event& event, const sf::Vector2f& mousePos) override;
    void onDraw(sf::RenderWindow& window, const sf::Vector2f& basePos) override;

private:
    Champion* m_champion;

    // Data
    std::vector<ItemTemplate> m_allItems;
    ItemCategory m_selectedCategory;
    int m_selectedItemIndex;

    // Helpers
    std::vector<ItemTemplate> getItemsInCategory(ItemCategory cat) const;
};
