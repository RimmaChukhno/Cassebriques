#pragma once

#include <SFML/Graphics.hpp>

#include <memory>

#include "Assets.hpp"
#include "Scene.hpp"
#include "Settings.hpp"

struct AppContext
{
    sf::RenderWindow &window;
    Assets &assets;
    Settings &settings;
    SceneId &requestedScene;
};

class App
{
public:
    App();
    void run();

private:
    sf::RenderWindow window;
    Assets assets;
    Settings settings;

    SceneId currentSceneId = SceneId::MainMenu;
    SceneId requestedSceneId = SceneId::MainMenu;
    AppContext ctx;
    std::unique_ptr<IScene> scene;

    std::unique_ptr<IScene> createScene(SceneId id);
};


