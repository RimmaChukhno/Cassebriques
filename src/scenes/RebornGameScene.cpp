#include "../app/App.hpp"
#include "../ui/Button.hpp"

#include "../game_reborn/Brick.hpp"
#include "../game_reborn/Cannon.hpp"
#include "../game_reborn/Projectile.hpp"

#include <SFML/Graphics.hpp>

#include <algorithm>
#include <cmath>
#include <memory>
#include <sstream>
#include <vector>

namespace
{
constexpr float WINDOW_W = 800.0f;
constexpr float WINDOW_H = 600.0f;

constexpr int BRICK_ROWS = 6;
constexpr int BRICK_COLS = 10;

int projectileBudget(Difficulty d)
{
    switch (d)
    {
    case Difficulty::Easy:
        return 70;
    case Difficulty::Hard:
        return 35;
    case Difficulty::Normal:
    default:
        return 50;
    }
}

float projectileSpeed(Difficulty d)
{
    switch (d)
    {
    case Difficulty::Easy:
        return 460.0f;
    case Difficulty::Hard:
        return 560.0f;
    case Difficulty::Normal:
    default:
        return 500.0f;
    }
}

int brickHpForRow(Difficulty d, int rowFromTop)
{
    // rowFromTop: 0..rows-1, top row hardest
    int base = 1 + (BRICK_ROWS - 1 - rowFromTop) / 2; // 1..3-ish
    if (d == Difficulty::Easy)
        base = std::max(1, base - 1);
    if (d == Difficulty::Hard)
        base = std::min(4, base + 1);
    return std::max(1, std::min(4, base));
}

} // namespace

class RebornGameScene final : public IScene
{
public:
    explicit RebornGameScene(AppContext &ctx)
        : IScene(ctx),
          cannon(WINDOW_W, WINDOW_H)
    {
        budget = projectileBudget(ctx.settings.difficulty);
        resetLevel();

        const sf::Font *font = ctx.assets.uiFontLoaded ? &ctx.assets.uiFont : nullptr;
        btnResume = Button(font, "Resume", {WINDOW_W / 2.0f - 140.0f, WINDOW_H / 2.0f + 10.0f}, {280.0f, 56.0f});
        btnRestart = Button(font, "Restart", {WINDOW_W / 2.0f - 140.0f, WINDOW_H / 2.0f + 80.0f}, {280.0f, 56.0f});
        btnBack = Button(font, "Back to Menu", {WINDOW_W / 2.0f - 140.0f, WINDOW_H / 2.0f + 150.0f}, {280.0f, 56.0f});
    }

    void handleEvent(const sf::Event &event) override
    {
        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Escape)
            {
                if (state == State::Playing)
                    state = State::Paused;
                else if (state == State::Paused)
                    state = State::Playing;
            }

            if (event.key.code == sf::Keyboard::Space)
            {
                if (state == State::Win || state == State::Lose)
                    resetAll();
            }
        }

        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (event.mouseButton.button == sf::Mouse::Left)
                pendingFire = true;
        }
    }

    void update(float dt) override
    {
        const sf::Vector2i mp = sf::Mouse::getPosition(ctx.window);
        const sf::Vector2f mpos(static_cast<float>(mp.x), static_cast<float>(mp.y));
        const bool mouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Left);

        if (state == State::Paused || state == State::Win || state == State::Lose)
        {
            btnResume.setSelected(state == State::Paused);
            btnResume.update(mpos, mouseDown);
            btnRestart.update(mpos, mouseDown);
            btnBack.update(mpos, mouseDown);

            if (btnResume.consumeClick() && state == State::Paused)
                state = State::Playing;
            if (btnRestart.consumeClick())
                resetAll();
            if (btnBack.consumeClick())
                ctx.requestedScene = SceneId::MainMenu;
            return;
        }

        // Aim cannon
        cannon.pointAt(mpos.x, mpos.y);

        // Fire (one projectile at a time for now)
        if (pendingFire && !hasActiveProjectile && used < budget)
        {
            fire();
            pendingFire = false;
        }
        pendingFire = false;

        // Update projectiles
        for (auto &p : projectiles)
            p.update(dt);

        // Remove lost projectiles
        projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(),
                                         [this](const Projectile &p)
                                         {
                                             if (p.isLost())
                                             {
                                                 hasActiveProjectile = false;
                                                 return true;
                                             }
                                             return false;
                                         }),
                          projectiles.end());

        // Collisions projectile-bricks
        for (auto &p : projectiles)
        {
            for (auto &b : bricks)
            {
                if (b.isDestroyed())
                    continue;
                if (p.checkCollision(b))
                {
                    // Reflect projectile by flipping velocity (fast, ok for now)
                    sf::Vector2f v = p.getVelocity();
                    p.setVelocity(-v.x, -v.y);

                    b.takeDamage(1);

                    // Points: hit + break bonus
                    score += 5;
                    if (b.isDestroyed())
                        score += b.getMaxHP() * 10;
                    break;
                }
            }
        }

        // Win condition
        if (std::all_of(bricks.begin(), bricks.end(), [](const RebornGame::Brick &b)
                        { return b.isDestroyed(); }))
        {
            state = State::Win;
            return;
        }

        // Lose condition: no budget remaining and nothing active
        if (used >= budget && !hasActiveProjectile && projectiles.empty())
        {
            state = State::Lose;
        }
    }

    void render(sf::RenderTarget &target) override
    {
        sf::RectangleShape bg(sf::Vector2f(WINDOW_W, WINDOW_H));
        bg.setFillColor(sf::Color(10, 10, 18));
        target.draw(bg);

        for (auto &b : bricks)
            if (!b.isDestroyed())
                b.draw(static_cast<sf::RenderWindow &>(target));

        for (auto &p : projectiles)
            p.draw(static_cast<sf::RenderWindow &>(target));

        cannon.draw(static_cast<sf::RenderWindow &>(target));

        drawHud(target);

        if (state == State::Paused)
        {
            drawOverlay(target, "PAUSED", "Resume / Restart / Back");
            btnResume.render(target);
            btnRestart.render(target);
            btnBack.render(target);
        }
        else if (state == State::Win)
        {
            drawOverlay(target, "VICTORY!", "Space: Restart");
            btnRestart.render(target);
            btnBack.render(target);
        }
        else if (state == State::Lose)
        {
            drawOverlay(target, "DEFEAT", "Out of projectiles");
            btnRestart.render(target);
            btnBack.render(target);
        }
    }

private:
    enum class State
    {
        Playing,
        Paused,
        Win,
        Lose,
    };

    Cannon cannon;
    std::vector<Projectile> projectiles;
    std::vector<RebornGame::Brick> bricks;

    int score = 0;
    int budget = 50;
    int used = 0;
    bool hasActiveProjectile = false;
    bool pendingFire = false;
    State state = State::Playing;

    Button btnResume;
    Button btnRestart;
    Button btnBack;

    void resetLevel()
    {
        bricks.clear();
        projectiles.clear();
        score = 0;
        used = 0;
        hasActiveProjectile = false;
        pendingFire = false;
        state = State::Playing;

        const float brickW = 72.0f;
        const float brickH = 28.0f;
        const float spacing = 6.0f;
        const float totalW = BRICK_COLS * brickW + (BRICK_COLS - 1) * spacing;
        const float startX = (WINDOW_W - totalW) / 2.0f;
        const float startY = 80.0f;

        for (int r = 0; r < BRICK_ROWS; r++)
        {
            for (int c = 0; c < BRICK_COLS; c++)
            {
                const float x = startX + c * (brickW + spacing);
                const float y = startY + r * (brickH + spacing);

                const int hp = brickHpForRow(ctx.settings.difficulty, r);
                bricks.emplace_back(x, y, brickW, brickH, hp);
            }
        }
    }

    void resetAll()
    {
        budget = projectileBudget(ctx.settings.difficulty);
        resetLevel();
    }

    void fire()
    {
        const float angle = cannon.getDirectionRadians();
        const sf::Vector2f pos = cannon.getPosition();
        const float speed = projectileSpeed(ctx.settings.difficulty);

        const float offset = 34.0f;
        const float ox = offset * std::cos(angle);
        const float oy = offset * std::sin(angle);

        projectiles.emplace_back(pos.x + ox, pos.y + oy, angle, WINDOW_W, WINDOW_H, speed);
        hasActiveProjectile = true;
        used++;
    }

    void drawHud(sf::RenderTarget &target)
    {
        if (!ctx.assets.uiFontLoaded)
            return;

        std::ostringstream ss;
        ss << "Score: " << score << "    Projectiles: " << (budget - used) << "/" << budget << "    (Click to fire)";
        sf::Text t(ss.str(), ctx.assets.uiFont, 18);
        t.setFillColor(sf::Color(220, 220, 235));
        t.setPosition(12.0f, 10.0f);
        target.draw(t);
    }

    void drawOverlay(sf::RenderTarget &target, const std::string &title, const std::string &subtitle)
    {
        sf::RectangleShape shade(sf::Vector2f(WINDOW_W, WINDOW_H));
        shade.setFillColor(sf::Color(0, 0, 0, 180));
        target.draw(shade);

        sf::RectangleShape panel(sf::Vector2f(520.0f, 300.0f));
        panel.setPosition(WINDOW_W / 2.0f - 260.0f, WINDOW_H / 2.0f - 200.0f);
        panel.setFillColor(sf::Color(25, 25, 40, 240));
        panel.setOutlineThickness(2.0f);
        panel.setOutlineColor(sf::Color(255, 200, 0));
        target.draw(panel);

        if (!ctx.assets.uiFontLoaded)
            return;

        sf::Text h(title, ctx.assets.uiFont, 56);
        h.setFillColor(sf::Color(255, 200, 0));
        h.setStyle(sf::Text::Bold);
        sf::FloatRect b = h.getLocalBounds();
        h.setOrigin(b.left + b.width / 2.0f, b.top + b.height / 2.0f);
        h.setPosition(WINDOW_W / 2.0f, WINDOW_H / 2.0f - 160.0f);
        target.draw(h);

        sf::Text s(subtitle, ctx.assets.uiFont, 18);
        s.setFillColor(sf::Color(180, 180, 200));
        b = s.getLocalBounds();
        s.setOrigin(b.left + b.width / 2.0f, b.top + b.height / 2.0f);
        s.setPosition(WINDOW_W / 2.0f, WINDOW_H / 2.0f - 120.0f);
        target.draw(s);
    }
};

std::unique_ptr<IScene> makeRebornGameScene(AppContext &ctx)
{
    return std::make_unique<RebornGameScene>(ctx);
}


