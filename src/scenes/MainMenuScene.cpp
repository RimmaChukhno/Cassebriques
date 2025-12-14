#include "../app/App.hpp"
#include "../ui/Button.hpp"

#include <SFML/Graphics.hpp>
#include <memory>

class MainMenuScene final : public IScene
{
public:
    explicit MainMenuScene(AppContext &ctx)
        : IScene(ctx)
    {
        const sf::Font *font = ctx.assets.uiFontLoaded ? &ctx.assets.uiFont : nullptr;

        if (font)
        {
            title.setFont(*font);
            title.setString("CASSE-BRIQUES");
            title.setCharacterSize(64);
            title.setFillColor(sf::Color(255, 200, 0));
            title.setStyle(sf::Text::Bold);

            sf::FloatRect b = title.getLocalBounds();
            title.setOrigin(b.left + b.width / 2.0f, b.top + b.height / 2.0f);
            title.setPosition(400.0f, 90.0f);

            subtitle.setFont(*font);
            subtitle.setString("Menu Principal");
            subtitle.setCharacterSize(28);
            subtitle.setFillColor(sf::Color(210, 210, 220));
            b = subtitle.getLocalBounds();
            subtitle.setOrigin(b.left + b.width / 2.0f, b.top + b.height / 2.0f);
            subtitle.setPosition(400.0f, 145.0f);
        }

        const float w = 460.0f;
        const float h = 60.0f;
        const float x = 400.0f - w / 2.0f;
        const float y0 = 230.0f;
        const float dy = 78.0f;

        btnClassic = Button(font, "Start - Classic", {x, y0 + dy * 0}, {w, h});
        btnReborn = Button(font, "Start - Reborn", {x, y0 + dy * 1}, {w, h});
        btnSettings = Button(font, "Settings", {x, y0 + dy * 2}, {w, h});
        btnQuit = Button(font, "Quit", {x, y0 + dy * 3}, {w, h});

        setSelected(0);
    }

    void handleEvent(const sf::Event &event) override
    {
        if (event.type != sf::Event::KeyPressed)
            return;

        if (event.key.code == sf::Keyboard::Up)
            setSelected((selected + 3) % 4);
        else if (event.key.code == sf::Keyboard::Down)
            setSelected((selected + 1) % 4);
        else if (event.key.code == sf::Keyboard::Escape)
            ctx.requestedScene = SceneId::Quit;
        else if (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Space)
            activateSelected();
    }

    void update(float) override
    {
        const sf::Vector2i mp = sf::Mouse::getPosition(ctx.window);
        const sf::Vector2f mpos(static_cast<float>(mp.x), static_cast<float>(mp.y));
        const bool mouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Left);

        btnClassic.update(mpos, mouseDown);
        btnReborn.update(mpos, mouseDown);
        btnSettings.update(mpos, mouseDown);
        btnQuit.update(mpos, mouseDown);

        if (btnClassic.consumeClick())
            ctx.requestedScene = SceneId::ClassicGame;
        else if (btnReborn.consumeClick())
            ctx.requestedScene = SceneId::RebornGame;
        else if (btnSettings.consumeClick())
            ctx.requestedScene = SceneId::Settings;
        else if (btnQuit.consumeClick())
            ctx.requestedScene = SceneId::Quit;

        // Hover also updates keyboard selection (nice UX)
        if (btnClassic.isHovered())
            setSelected(0);
        else if (btnReborn.isHovered())
            setSelected(1);
        else if (btnSettings.isHovered())
            setSelected(2);
        else if (btnQuit.isHovered())
            setSelected(3);
    }

    void render(sf::RenderTarget &target) override
    {
        // Decorative line
        sf::RectangleShape top(sf::Vector2f(800.0f, 3.0f));
        top.setFillColor(sf::Color(255, 200, 0));
        target.draw(top);

        if (ctx.assets.uiFontLoaded)
        {
            target.draw(title);
            target.draw(subtitle);
        }

        sf::RectangleShape sep(sf::Vector2f(620.0f, 2.0f));
        sep.setPosition(90.0f, 190.0f);
        sep.setFillColor(sf::Color(90, 90, 110));
        target.draw(sep);

        btnClassic.render(target);
        btnReborn.render(target);
        btnSettings.render(target);
        btnQuit.render(target);

        if (ctx.assets.uiFontLoaded)
        {
            sf::Text hint("Mouse or Up/Down + Enter. Esc = Quit", ctx.assets.uiFont, 16);
            hint.setFillColor(sf::Color(150, 150, 170));
            sf::FloatRect b = hint.getLocalBounds();
            hint.setOrigin(b.left + b.width / 2.0f, b.top + b.height / 2.0f);
            hint.setPosition(400.0f, 570.0f);
            target.draw(hint);
        }
    }

private:
    sf::Text title;
    sf::Text subtitle;

    Button btnClassic;
    Button btnReborn;
    Button btnSettings;
    Button btnQuit;

    int selected = 0;

    void setSelected(int i)
    {
        selected = i;
        btnClassic.setSelected(selected == 0);
        btnReborn.setSelected(selected == 1);
        btnSettings.setSelected(selected == 2);
        btnQuit.setSelected(selected == 3);
    }

    void activateSelected()
    {
        switch (selected)
        {
        case 0:
            ctx.requestedScene = SceneId::ClassicGame;
            break;
        case 1:
            ctx.requestedScene = SceneId::RebornGame;
            break;
        case 2:
            ctx.requestedScene = SceneId::Settings;
            break;
        case 3:
        default:
            ctx.requestedScene = SceneId::Quit;
            break;
        }
    }
};

std::unique_ptr<IScene> makeMainMenuScene(AppContext &ctx)
{
    return std::make_unique<MainMenuScene>(ctx);
}


