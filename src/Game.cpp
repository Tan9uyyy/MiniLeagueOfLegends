#include "Game.hpp"
#include "ClickIndicator.hpp"
#include "Nexus.hpp"
#include "Turret.hpp"

Game::Game() {
  // 1. Fenêtre en plein écran
  sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
  m_window.create(desktopMode, "Mini League of Legends", sf::State::Fullscreen);
  m_window.setFramerateLimit(60);

  // 2. Vues (caméra + minimap)
  initViews();

  // 3. Police d'écriture
  m_fontLoaded =
      m_font.openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf") ||
      m_font.openFromFile(
          "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf") ||
      m_font.openFromFile("/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf");

  // 4. Monde
  initWorld();

  // 5. Interface
  m_hud = std::make_unique<HUD>(m_champion, m_font);
  m_shopUI = std::make_unique<ShopUI>(m_champion, m_font);
  m_settingsUI = std::make_unique<SettingsUI>(m_font);

  // Enregistrer les fenêtres UI (ordre = priorité d'interception, du plus prioritaire au moins)
  m_uiWindows.push_back(m_settingsUI.get());
  m_uiWindows.push_back(m_shopUI.get());
}

void Game::initViews() {
  sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();

  m_camera = sf::View(sf::Vector2f(0.0f, 0.0f),
                      sf::Vector2f(desktopMode.size.x, desktopMode.size.y));

  m_minimapView = sf::View(sf::Vector2f(5000.0f, 5000.0f),
                           sf::Vector2f(10000.0f, 10000.0f));

  float minimapSize = 350.0f;
  float viewportW = minimapSize / desktopMode.size.x;
  float viewportH = minimapSize / desktopMode.size.y;

  m_minimapView.setViewport(sf::FloatRect(1.0f - viewportW - 0.01f,
                                          1.0f - viewportH - 0.01f, viewportW,
                                          viewportH));
}

void Game::initWorld() {
  auto mapPtr = std::make_unique<Map>(10000.0f, 10000.0f);
  m_gameMap = mapPtr.get();

  // Nexus Allié
  auto alliedNexus =
      std::make_unique<Nexus>(sf::Vector2f(1000.0f, 9000.0f), Team::ALLIED);
  m_gameMap->addObstacle(alliedNexus->getBounds());
  m_entities.push_back(std::move(alliedNexus));

  // Tourelles Alliées
  auto turretA1 =
      std::make_unique<Turret>(sf::Vector2f(1200.0f, 8500.0f), Team::ALLIED);
  auto turretA2 =
      std::make_unique<Turret>(sf::Vector2f(1500.0f, 8800.0f), Team::ALLIED);
  m_gameMap->addObstacle(turretA1->getBounds());
  m_gameMap->addObstacle(turretA2->getBounds());
  m_entities.push_back(std::move(turretA1));
  m_entities.push_back(std::move(turretA2));

  // Nexus Ennemi (rapproché pour les tests)
  auto enemyNexus =
      std::make_unique<Nexus>(sf::Vector2f(2500.0f, 8000.0f), Team::ENEMY);
  m_gameMap->addObstacle(enemyNexus->getBounds());
  m_entities.push_back(std::move(enemyNexus));

  // Tourelles Ennemies
  auto turretE1 =
      std::make_unique<Turret>(sf::Vector2f(2200.0f, 8500.0f), Team::ENEMY);
  auto turretE2 =
      std::make_unique<Turret>(sf::Vector2f(2500.0f, 8300.0f), Team::ENEMY);
  m_gameMap->addObstacle(turretE1->getBounds());
  m_gameMap->addObstacle(turretE2->getBounds());
  m_entities.push_back(std::move(turretE1));
  m_entities.push_back(std::move(turretE2));

  // Carte (en fond)
  m_entities.push_back(std::move(mapPtr));

  // Indicateur de clic
  m_entities.push_back(std::make_unique<ClickIndicator>());

  // Champion
  auto championPtr = std::make_unique<Champion>(sf::Vector2f(1300.0f, 8800.0f),
                                                *m_gameMap, Team::ALLIED);
  m_champion = championPtr.get();
  m_entities.push_back(std::move(championPtr));
}

void Game::run() {
  sf::Clock clock;
  while (m_window.isOpen()) {
    float deltaTime = clock.restart().asSeconds();
    processEvents();
    update(deltaTime);
    render();
  }
}

// --- Factorisation : une seule boucle sur toutes les fenêtres UI ---
bool Game::dispatchMouseEventToWindows(const sf::Event& event) {
  bool isMouseEvent = (event.type == sf::Event::MouseButtonPressed ||
                       event.type == sf::Event::MouseButtonReleased ||
                       event.type == sf::Event::MouseMoved);
  if (!isMouseEvent) return false;

  sf::Vector2i pixelPos;
  if (event.type == sf::Event::MouseMoved) {
    pixelPos = sf::Vector2i(event.mouseMove.x, event.mouseMove.y);
  } else {
    pixelPos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
  }

  for (auto* win : m_uiWindows) {
    if (win && win->isOpen()) {
      UIAction action = win->handleEvent(event, pixelPos, m_window);
      if (action == UIAction::QUIT) {
        m_window.close();
        return true;
      }
      if (action == UIAction::CONSUMED) {
        return true;
      }
    }
  }

  return false;
}

void Game::processEvents() {
  sf::Event event;
  while (m_window.pollEvent(event)) {
    if (event.type == sf::Event::Closed)
      m_window.close();

    // Raccourcis clavier
    if (event.type == sf::Event::KeyPressed) {
      if (event.key.code == sf::Keyboard::Escape && m_settingsUI)
        m_settingsUI->toggle();
      if (event.key.code == sf::Keyboard::P && m_shopUI)
        m_shopUI->toggle();

      // Debug : Level Up avec L
      if (event.key.code == sf::Keyboard::L && m_champion)
        m_champion->debugLevelUp();

      // Sorts : Ctrl+Touche = upgrade, Touche seule = cast
      if (m_champion) {
        bool ctrl = event.key.control;
        if (event.key.code == sf::Keyboard::Key::A) {
          if (ctrl) m_champion->upgradeSpell(0);
          else m_champion->castSpell(0);
        }
        if (event.key.code == sf::Keyboard::Key::Z) {
          if (ctrl) m_champion->upgradeSpell(1);
          else m_champion->castSpell(1);
        }
        if (event.key.code == sf::Keyboard::Key::E) {
          if (ctrl) m_champion->upgradeSpell(2);
          else m_champion->castSpell(2);
        }
        if (event.key.code == sf::Keyboard::Key::R) {
          if (ctrl) m_champion->upgradeSpell(3);
          else m_champion->castSpell(3);
        }
      }
    }

    // Dispatch souris aux fenêtres UI (factorisé !)
    if (dispatchMouseEventToWindows(event))
      continue;

    // Clic gauche sur le bouton BOUTIQUE du HUD
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Button::Left) {
      sf::Vector2i pixelPos(event.mouseButton.x, event.mouseButton.y);
      sf::Vector2f uiPos =
          m_window.mapPixelToCoords(pixelPos, m_window.getDefaultView());
      if (m_hud && m_hud->isShopButtonClicked(uiPos)) {
        if (m_shopUI) m_shopUI->toggle();
        continue;
      }
    }

    // Clic droit dans le monde
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Right) {
      sf::Vector2i pixelPos(event.mouseButton.x, event.mouseButton.y);
      sf::Vector2f worldPos = m_window.mapPixelToCoords(pixelPos, m_camera);

      bool enemyClicked = false;

      for (auto& entity : m_entities) {
        if (auto combatEntity = dynamic_cast<CombatEntity*>(entity.get())) {
          if (combatEntity->getTeam() != Team::ALLIED &&
              combatEntity->getBounds().contains(worldPos) &&
              !combatEntity->isDead()) {
            m_champion->setTargetEntity(combatEntity);
            enemyClicked = true;
            break;
          }
        }
      }

      for (auto& entity : m_entities) {
        if (enemyClicked && entity.get() == m_champion)
          continue;
        entity->setTargetPosition(worldPos);
      }
    }
  }
}

void Game::update(float deltaTime) {
  if (m_champion) {
    m_camera.setCenter(m_champion->getPosition());
  }

  for (auto& entity : m_entities) {
    entity->update(deltaTime);
  }
}

void Game::render() {
  m_window.clear(sf::Color(30, 30, 30));

  m_renderer.renderWorld(m_window, m_camera, m_entities);
  m_renderer.renderMinimap(m_window, m_minimapView, m_entities);

  if (m_fontLoaded) {
    m_renderer.renderUI(m_window, m_hud.get(), m_uiWindows);
  }

  m_window.display();
}
