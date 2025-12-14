#include "../app/App.hpp"
#include "../ui/Button.hpp"

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

static const char *difficultyLabel(Difficulty d)
{
    switch (d)
    {
    case Difficulty::Easy:
        return "Easy";
    case Difficulty::Hard:
        return "Hard";
    case Difficulty::Normal:
    default:
        return "Normal";
    }
}

static Difficulty nextDifficulty(Difficulty d)
{
    switch (d)
    {
    case Difficulty::Easy:
        return Difficulty::Normal;
    case Difficulty::Normal:
        return Difficulty::Hard;
    case Difficulty::Hard:
    default:
        return Difficulty::Easy;
    }
}

static float clamp01(float v)
{
    if (v < 0.0f)
        return 0.0f;
    if (v > 1.0f)
        return 1.0f;
    return v;
}

class SettingsScene final : public IScene
{
public:
    explicit SettingsScene(AppContext &ctx)
        : IScene(ctx)
    {
        const sf::Font *font = ctx.assets.uiFontLoaded ? &ctx.assets.uiFont : nullptr;

        if (font)
        {
            title.setFont(*font);
            title.setString("SETTINGS");
            title.setCharacterSize(56);
            title.setFillColor(sf::Color(255, 200, 0));
            title.setStyle(sf::Text::Bold);
            sf::FloatRect b = title.getLocalBounds();
            title.setOrigin(b.left + b.width / 2.0f, b.top + b.height / 2.0f);
            title.setPosition(400.0f, 90.0f);

            labelVolume.setFont(*font);
            labelVolume.setCharacterSize(26);
            labelVolume.setFillColor(sf::Color(220, 220, 235));
            labelVolume.setPosition(170.0f, 200.0f);

            labelDifficulty.setFont(*font);
            labelDifficulty.setCharacterSize(26);
            labelDifficulty.setFillColor(sf::Color(220, 220, 235));
            labelDifficulty.setPosition(170.0f, 290.0f);
        }

        btnVolMinus = Button(font, "-", {520.0f, 195.0f}, {60.0f, 48.0f});
        btnVolPlus = Button(font, "+", {590.0f, 195.0f}, {60.0f, 48.0f});

        btnDiffCycle = Button(font, "Change", {520.0f, 285.0f}, {130.0f, 48.0f});

        btnBack = Button(font, "Back", {170.0f, 470.0f}, {220.0f, 60.0f});

        syncLabels();
    }

    void handleEvent(const sf::Event &event) override
    {
        if (event.type != sf::Event::KeyPressed)
            return;

        if (event.key.code == sf::Keyboard::Escape)
        {
            ctx.settings.saveToFile("settings.ini");
            ctx.requestedScene = SceneId::MainMenu;
        }
        else if (event.key.code == sf::Keyboard::Left)
        {
            ctx.settings.masterVolume = clamp01(ctx.settings.masterVolume - 0.05f);
            syncLabels();
        }
        else if (event.key.code == sf::Keyboard::Right)
        {
            ctx.settings.masterVolume = clamp01(ctx.settings.masterVolume + 0.05f);
            syncLabels();
        }
        else if (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Space)
        {
            // Shortcut: cycle difficulty
            ctx.settings.difficulty = nextDifficulty(ctx.settings.difficulty);
            syncLabels();
        }
    }

    void update(float) override
    {
        const sf::Vector2i mp = sf::Mouse::getPosition(ctx.window);
        const sf::Vector2f mpos(static_cast<float>(mp.x), static_cast<float>(mp.y));
        const bool mouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Left);

        btnVolMinus.update(mpos, mouseDown);
        btnVolPlus.update(mpos, mouseDown);
        btnDiffCycle.update(mpos, mouseDown);
        btnBack.update(mpos, mouseDown);

        if (btnVolMinus.consumeClick())
        {
            ctx.settings.masterVolume = clamp01(ctx.settings.masterVolume - 0.05f);
            syncLabels();
        }
        else if (btnVolPlus.consumeClick())
        {
            ctx.settings.masterVolume = clamp01(ctx.settings.masterVolume + 0.05f);
            syncLabels();
        }
        else if (btnDiffCycle.consumeClick())
        {
            ctx.settings.difficulty = nextDifficulty(ctx.settings.difficulty);
            syncLabels();
        }
        else if (btnBack.consumeClick())
        {
            ctx.settings.saveToFile("settings.ini");
            ctx.requestedScene = SceneId::MainMenu;
        }
    }

    void render(sf::RenderTarget &target) override
    {
        sf::RectangleShape top(sf::Vector2f(800.0f, 3.0f));
        top.setFillColor(sf::Color(255, 200, 0));
        target.draw(top);

        if (ctx.assets.uiFontLoaded)
        {
            target.draw(title);
            target.draw(labelVolume);
            target.draw(labelDifficulty);

            sf::Text hint("Left/Right: volume, Enter: cycle difficulty, Esc: back", ctx.assets.uiFont, 16);
            hint.setFillColor(sf::Color(150, 150, 170));
            sf::FloatRect b = hint.getLocalBounds();
            hint.setOrigin(b.left + b.width / 2.0f, b.top + b.height / 2.0f);
            hint.setPosition(400.0f, 570.0f);
            target.draw(hint);
        }

        btnVolMinus.render(target);
        btnVolPlus.render(target);
        btnDiffCycle.render(target);
        btnBack.render(target);
    }

private:
    sf::Text title;
    sf::Text labelVolume;
    sf::Text labelDifficulty;

    Button btnVolMinus;
    Button btnVolPlus;
    Button btnDiffCycle;
    Button btnBack;

    void syncLabels()
    {
        if (!ctx.assets.uiFontLoaded)
            return;

        const int volPct = static_cast<int>(ctx.settings.masterVolume * 100.0f + 0.5f);
        labelVolume.setString("Master Volume: " + std::to_string(volPct) + "%");
        labelDifficulty.setString(std::string("Difficulty: ") + difficultyLabel(ctx.settings.difficulty));
    }
};

std::unique_ptr<IScene> makeSettingsScene(AppContext &ctx)
{
    return std::make_unique<SettingsScene>(ctx);
}


