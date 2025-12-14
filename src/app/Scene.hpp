#pragma once

#include <SFML/Graphics.hpp>

struct AppContext;

enum class SceneId
{
    MainMenu,
    Settings,
    ClassicGame,
    RebornGame,
    Quit,
};

class IScene
{
public:
    explicit IScene(AppContext &ctx) : ctx(ctx) {}
    virtual ~IScene() = default;

    virtual void handleEvent(const sf::Event &event) = 0;
    virtual void update(float dt) = 0;
    virtual void render(sf::RenderTarget &target) = 0;

protected:
    AppContext &ctx;
};


