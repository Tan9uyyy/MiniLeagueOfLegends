#include "ShopUI.hpp"

ShopUI::ShopUI(Champion* champion, const sf::Font& font)
    : UIWindow(font, "BOUTIQUE", sf::Vector2f(600.0f, 400.0f), sf::Vector2f(100.0f, 100.0f)),
      m_champion(champion), m_selectedCategory(ItemCategory::DAMAGE), m_selectedItemIndex(-1)
{
    m_allItems = ItemTemplate::getShopItems();
}

std::vector<ItemTemplate> ShopUI::getItemsInCategory(ItemCategory cat) const {
    std::vector<ItemTemplate> filtered;
    for (const auto& item : m_allItems) {
        if (item.category == cat) {
            filtered.push_back(item);
        }
    }
    return filtered;
}

UIAction ShopUI::onHandleEvent(const sf::Event& event, const sf::Vector2f& mousePos) {
    sf::Vector2f basePos = getBasePos();

    if (event.type == sf::Event::MouseButtonPressed &&
        (event.mouseButton.button == sf::Mouse::Left || event.mouseButton.button == sf::Mouse::Right)) {

        bool isLeft = (event.mouseButton.button == sf::Mouse::Left);
        bool isRight = (event.mouseButton.button == sf::Mouse::Right);

        // Catégories (gauche)
        sf::FloatRect damageCat(basePos.x + 10, basePos.y + 40, 100, 30);
        sf::FloatRect asCat(basePos.x + 10, basePos.y + 80, 100, 30);
        sf::FloatRect defCat(basePos.x + 10, basePos.y + 120, 100, 30);

        if (isLeft) {
            if (damageCat.contains(mousePos)) { m_selectedCategory = ItemCategory::DAMAGE; m_selectedItemIndex = -1; return UIAction::CONSUMED; }
            if (asCat.contains(mousePos)) { m_selectedCategory = ItemCategory::ATTACK_SPEED; m_selectedItemIndex = -1; return UIAction::CONSUMED; }
            if (defCat.contains(mousePos)) { m_selectedCategory = ItemCategory::DEFENSE; m_selectedItemIndex = -1; return UIAction::CONSUMED; }
        }

        // Liste des objets
        auto items = getItemsInCategory(m_selectedCategory);
        for (size_t i = 0; i < items.size(); ++i) {
            sf::FloatRect itemRect(basePos.x + 130, basePos.y + 40 + i * 40, 200, 30);
            if (itemRect.contains(mousePos)) {
                int realIndex = -1;
                for (size_t j = 0; j < m_allItems.size(); ++j) {
                    if (m_allItems[j].id == items[i].id) { realIndex = j; break; }
                }
                if (realIndex != -1) {
                    m_selectedItemIndex = realIndex;
                    if (isRight) m_champion->buyItem(m_allItems[realIndex]);
                    return UIAction::CONSUMED;
                }
            }
        }

        // Inventaire (clic droit pour vendre)
        const auto& inv = m_champion->getInventory();
        for (int i = 0; i < (int)inv.size(); ++i) {
            sf::FloatRect slotBounds(basePos.x + 100 + i * 50, basePos.y + 345, 40, 40);
            if (slotBounds.contains(mousePos)) {
                if (isRight) m_champion->sellItem(i);
                return UIAction::CONSUMED;
            }
        }

        // Boutons Acheter / Vendre
        if (isLeft && m_selectedItemIndex != -1) {
            sf::FloatRect buyBtn(basePos.x + 350, basePos.y + 300, 100, 40);
            if (buyBtn.contains(mousePos)) {
                m_champion->buyItem(m_allItems[m_selectedItemIndex]);
                return UIAction::CONSUMED;
            }

            int invIdx = m_champion->getInventoryIndexOf(m_allItems[m_selectedItemIndex].id);
            if (invIdx != -1) {
                sf::FloatRect sellBtn(basePos.x + 460, basePos.y + 300, 100, 40);
                if (sellBtn.contains(mousePos)) {
                    m_champion->sellItem(invIdx);
                    return UIAction::CONSUMED;
                }
            }
        }
    }

    return UIAction::NONE;
}

void ShopUI::onDraw(sf::RenderWindow& window, const sf::Vector2f& basePos) {
    const sf::Font& font = getFont();

    // 1. Catégories (Gauche)
    sf::RectangleShape catBtn(sf::Vector2f(100.0f, 30.0f));
    sf::Text catText("", font, 14);
    std::string cats[] = {"Degats", "Vitesse", "Defense"};
    ItemCategory catEnums[] = {ItemCategory::DAMAGE, ItemCategory::ATTACK_SPEED, ItemCategory::DEFENSE};

    for (int i = 0; i < 3; ++i) {
        catBtn.setPosition(basePos.x + 10, basePos.y + 40 + i * 40);
        catBtn.setFillColor(m_selectedCategory == catEnums[i] ? sf::Color(80, 80, 80) : sf::Color(40, 40, 40));
        catText.setString(cats[i]);
        catText.setPosition(basePos.x + 15, basePos.y + 45 + i * 40);
        window.draw(catBtn);
        window.draw(catText);
    }

    // 2. Liste des objets (Centre)
    auto items = getItemsInCategory(m_selectedCategory);
    sf::RectangleShape itemBg(sf::Vector2f(200.0f, 30.0f));
    sf::Text itemText("", font, 14);

    for (size_t i = 0; i < items.size(); ++i) {
        itemBg.setPosition(basePos.x + 130, basePos.y + 40 + i * 40);
        bool isSelected = (m_selectedItemIndex != -1 && m_allItems[m_selectedItemIndex].id == items[i].id);
        itemBg.setFillColor(isSelected ? sf::Color(100, 100, 50) : sf::Color(50, 50, 50));
        itemText.setString(items[i].name + " (" + std::to_string((int)items[i].price) + "G)");
        itemText.setPosition(basePos.x + 135, basePos.y + 45 + i * 40);
        window.draw(itemBg);
        window.draw(itemText);
    }

    // 3. Détails + Boutons (Droite)
    if (m_selectedItemIndex != -1) {
        const auto& selected = m_allItems[m_selectedItemIndex];

        sf::Text detailName(selected.name, font, 18);
        detailName.setFillColor(sf::Color::Yellow);
        detailName.setPosition(basePos.x + 350, basePos.y + 40);

        sf::Text detailDesc(selected.description, font, 14);
        detailDesc.setPosition(basePos.x + 350, basePos.y + 70);

        window.draw(detailName);
        window.draw(detailDesc);

        // Bouton Acheter
        sf::RectangleShape buyBtn(sf::Vector2f(100.0f, 40.0f));
        buyBtn.setPosition(basePos.x + 350, basePos.y + 300);
        buyBtn.setFillColor(sf::Color(39, 174, 96));
        sf::Text buyText("ACHETER", font, 14);
        buyText.setPosition(basePos.x + 365, basePos.y + 310);
        window.draw(buyBtn);
        window.draw(buyText);

        // Bouton Vendre (si possédé)
        int invIdx = m_champion->getInventoryIndexOf(selected.id);
        if (invIdx != -1) {
            sf::RectangleShape sellBtn(sf::Vector2f(100.0f, 40.0f));
            sellBtn.setPosition(basePos.x + 460, basePos.y + 300);
            sellBtn.setFillColor(sf::Color(192, 57, 43));
            sf::Text sellText("VENDRE", font, 14);
            sellText.setPosition(basePos.x + 475, basePos.y + 310);
            window.draw(sellBtn);
            window.draw(sellText);
        }
    }

    // 4. Inventaire (Bas)
    sf::Text invTitle("INVENTAIRE (Clic droit pour vendre)", font, 10);
    invTitle.setPosition(basePos.x + 10, basePos.y + 330);
    invTitle.setFillColor(sf::Color(200, 200, 200));
    window.draw(invTitle);

    const auto& inv = m_champion->getInventory();
    sf::RectangleShape slot(sf::Vector2f(40.0f, 40.0f));
    slot.setOutlineThickness(1.0f);
    slot.setOutlineColor(sf::Color(100, 100, 100));
    sf::Text slotText("", font, 10);

    for (int i = 0; i < 6; ++i) {
        slot.setPosition(basePos.x + 100 + i * 50, basePos.y + 345);
        if (i < (int)inv.size()) {
            slot.setFillColor(sf::Color(80, 80, 20));
            slotText.setString(inv[i].name.substr(0, 4));
            slotText.setPosition(basePos.x + 105 + i * 50, basePos.y + 360);
        } else {
            slot.setFillColor(sf::Color(30, 30, 30));
            slotText.setString("");
        }
        window.draw(slot);
        if (i < (int)inv.size()) window.draw(slotText);
    }
}
