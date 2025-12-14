#include "App.hpp"

#include <iostream>

// Scenes (implemented in src/scenes/)
std::unique_ptr<IScene> makeMainMenuScene(AppContext &ctx);
std::unique_ptr<IScene> makeSettingsScene(AppContext &ctx);
std::unique_ptr<IScene> makeClassicGameScene(AppContext &ctx);
std::unique_ptr<IScene> makeRebornGameScene(AppContext &ctx);

App::App()
    : window(sf::VideoMode(800, 600), "Casse-Briques"),
      ctx{window, assets, settings, requestedSceneId}
{
    window.setFramerateLimit(60);

    // Load settings + assets once
    settings = Settings::loadFromFile("settings.ini");
    assets.init();
}

std::unique_ptr<IScene> App::createScene(SceneId id)
{
    switch (id)
    {
    case SceneId::MainMenu:
        return makeMainMenuScene(ctx);
    case SceneId::Settings:
        return makeSettingsScene(ctx);
    case SceneId::ClassicGame:
        return makeClassicGameScene(ctx);
    case SceneId::RebornGame:
        return makeRebornGameScene(ctx);
    case SceneId::Quit:
    default:
        return nullptr;
    }
}

void App::run()
{
    requestedSceneId = currentSceneId;
    scene = createScene(currentSceneId);

    sf::Clock clock;
    while (window.isOpen())
    {
        // Scene transition requested?
        if (requestedSceneId != currentSceneId)
        {
            // Persist settings when leaving settings screen (safe to do often)
            settings.saveToFile("settings.ini");

            currentSceneId = requestedSceneId;
            if (currentSceneId == SceneId::Quit)
            {
                window.close();
                break;
            }
            scene = createScene(currentSceneId);
        }

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                break;
            }

            if (scene)
                scene->handleEvent(event);
        }

        const float dt = clock.restart().asSeconds();
        if (scene)
            scene->update(dt);

        window.clear(sf::Color(15, 15, 25));
        if (scene)
            scene->render(window);
        window.display();
    }
}


