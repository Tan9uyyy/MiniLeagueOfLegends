#pragma once
#include "Champion.hpp"
#include "Item.hpp"
#include "UIWindow.hpp"
#include <functional>
#include <vector>

class ShopUI : public UIWindow {
public:
  ShopUI(Champion *champion, const sf::Font &font);

  // Callbacks pour le réseau
  std::function<void(int)> onBuyItem;
  std::function<void(int)> onSellItem;

protected:
  UIAction onHandleEvent(const sf::Event &event,
                         const sf::Vector2f &mousePos) override;
  void onDraw(sf::RenderWindow &window, const sf::Vector2f &basePos) override;

private:
  Champion *m_champion;

  // Data
  std::vector<ItemTemplate> m_allItems;
  ItemCategory m_selectedCategory;
  int m_selectedItemIndex;

  // Helpers
  std::vector<ItemTemplate> getItemsInCategory(ItemCategory cat) const;
};
