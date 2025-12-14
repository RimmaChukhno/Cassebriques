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
#include <string>
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

    int maxActiveShots(Difficulty d)
    {
        switch (d)
        {
        case Difficulty::Easy:
            return 5;
        case Difficulty::Hard:
            return 3;
        case Difficulty::Normal:
        default:
            return 4;
        }
    }

    float fireCooldownSeconds(Difficulty d)
    {
        switch (d)
        {
        case Difficulty::Easy:
            return 0.18f;
        case Difficulty::Hard:
            return 0.28f;
        case Difficulty::Normal:
        default:
            return 0.22f;
        }
    }

    float brickDescendSpeed(Difficulty d)
    {
        switch (d)
        {
        case Difficulty::Easy:
            return 8.0f;
        case Difficulty::Hard:
            return 14.0f;
        case Difficulty::Normal:
        default:
            return 11.0f;
        }
    }

    int shotCost(Projectile::ShotType t)
    {
        switch (t)
        {
        case Projectile::ShotType::Piercing:
            return 2;
        case Projectile::ShotType::Explosive:
            return 3;
        case Projectile::ShotType::Normal:
        default:
            return 1;
        }
    }

    const char *shotName(Projectile::ShotType t)
    {
        switch (t)
        {
        case Projectile::ShotType::Piercing:
            return "Piercing (2)";
        case Projectile::ShotType::Explosive:
            return "Explosive (3)";
        case Projectile::ShotType::Normal:
        default:
            return "Normal (1)";
        }
    }

    float clampf(float v, float lo, float hi)
    {
        return std::max(lo, std::min(v, hi));
    }

    bool circleRectCollisionNormal(const Projectile &p, const RebornGame::Brick &b, sf::Vector2f &outNormal, float &outPenetration)
    {
        const sf::Vector2f c = p.getPosition();
        const float r = p.getRadius();
        const AABB box = b.getAABB();

        const float closestX = clampf(c.x, box.left, box.right);
        const float closestY = clampf(c.y, box.top, box.bottom);

        float dx = c.x - closestX;
        float dy = c.y - closestY;
        const float d2 = dx * dx + dy * dy;

        // If outside / touching but not intersecting, no collision
        if (d2 >= r * r)
            return false;

        // If the circle center is inside the rectangle (dx=dy=0), choose the nearest side.
        if (dx == 0.0f && dy == 0.0f)
        {
            const float distLeft = c.x - box.left;
            const float distRight = box.right - c.x;
            const float distTop = c.y - box.top;
            const float distBottom = box.bottom - c.y;

            const float minDist = std::min(std::min(distLeft, distRight), std::min(distTop, distBottom));
            if (minDist == distLeft)
                outNormal = sf::Vector2f(-1.0f, 0.0f);
            else if (minDist == distRight)
                outNormal = sf::Vector2f(1.0f, 0.0f);
            else if (minDist == distTop)
                outNormal = sf::Vector2f(0.0f, -1.0f);
            else
                outNormal = sf::Vector2f(0.0f, 1.0f);

            outPenetration = r + 1.0f;
            return true;
        }

        const float d = std::sqrt(d2);
        outNormal = sf::Vector2f(dx / d, dy / d);
        outPenetration = r - d;
        return true;
    }

    sf::Vector2f reflect(const sf::Vector2f &v, const sf::Vector2f &n)
    {
        const float dot = v.x * n.x + v.y * n.y;
        return sf::Vector2f(v.x - 2.0f * dot * n.x, v.y - 2.0f * dot * n.y);
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

            // Shot type hotkeys
            if (event.key.code == sf::Keyboard::Num1)
                currentShot = Projectile::ShotType::Normal;
            if (event.key.code == sf::Keyboard::Num2)
                currentShot = Projectile::ShotType::Piercing;
            if (event.key.code == sf::Keyboard::Num3)
                currentShot = Projectile::ShotType::Explosive;
        }

        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (event.mouseButton.button == sf::Mouse::Left)
                firingHeld = true;
        }
        if (event.type == sf::Event::MouseButtonReleased)
        {
            if (event.mouseButton.button == sf::Mouse::Left)
                firingHeld = false;
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

        // Fire cooldown
        if (fireCooldown > 0.0f)
            fireCooldown = std::max(0.0f, fireCooldown - dt);

        // Fire (multi-shot, arcade feel)
        const int cost = shotCost(currentShot);
        if (firingHeld && fireCooldown <= 0.0f && static_cast<int>(projectiles.size()) < maxActive && used + cost <= budget)
        {
            fire(currentShot);
            used += cost;
            fireCooldown = fireCooldownSeconds(ctx.settings.difficulty);
        }

        // Update projectiles
        for (auto &p : projectiles)
            p.update(dt);

        // Remove lost/dead projectiles (also update miss penalty)
        projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(),
                                         [this](const Projectile &p)
                                         {
                                             if (p.isLost() || p.isDead())
                                             {
                                                 if (!p.hasHitSomething())
                                                     combo = 0;
                                                 return true;
                                             }
                                             return false;
                                         }),
                          projectiles.end());

        // Bricks descend (pressure)
        const float descend = brickDescendSpeed(ctx.settings.difficulty) * dt;
        for (auto &b : bricks)
        {
            if (b.isDestroyed())
                continue;
            const sf::Vector2f pos = b.getPosition();
            b.setPosition(pos.x, pos.y + descend);
        }

        // Danger line lose condition
        for (const auto &b : bricks)
        {
            if (b.isDestroyed())
                continue;
            const sf::Vector2f pos = b.getPosition();
            const sf::Vector2f sz = b.getSize();
            if (pos.y + sz.y >= dangerLineY)
            {
                state = State::Lose;
                loseReason = LoseReason::DangerLine;
                return;
            }
        }

        // Collisions projectile-bricks
        for (auto &p : projectiles)
        {
            for (auto &b : bricks)
            {
                if (b.isDestroyed())
                    continue;
                if (p.checkCollision(b))
                {
                    p.markHit();

                    // Determine collision normal (for proper bounce and depenetration)
                    sf::Vector2f n(0.0f, -1.0f);
                    float pen = 0.0f;
                    circleRectCollisionNormal(p, b, n, pen);

                    // Damage & scoring
                    b.takeDamage(1);
                    score += 5;
                    if (b.isDestroyed())
                        score += b.getMaxHP() * 10;

                    // Combo: reward accurate shots
                    combo = std::min(combo + 1, 20);
                    score += combo; // small ramp

                    // Special shot behavior
                    if (p.getShotType() == Projectile::ShotType::Explosive)
                    {
                        const float R = p.getExplosionRadius();
                        const sf::Vector2f hitPos = p.getPosition();

                        for (auto &bb : bricks)
                        {
                            if (bb.isDestroyed())
                                continue;
                            const sf::Vector2f bp = bb.getPosition();
                            const sf::Vector2f bs = bb.getSize();
                            const sf::Vector2f bc(bp.x + bs.x / 2.0f, bp.y + bs.y / 2.0f);
                            const float dx = bc.x - hitPos.x;
                            const float dy = bc.y - hitPos.y;
                            if (dx * dx + dy * dy <= R * R)
                            {
                                const bool wasAlive = !bb.isDestroyed();
                                bb.takeDamage(1);
                                if (wasAlive && bb.isDestroyed())
                                    score += bb.getMaxHP() * 8;
                            }
                        }

                        // Explosive projectile disappears on hit
                        p.kill();
                        break;
                    }

                    if (p.getShotType() == Projectile::ShotType::Piercing && p.getPierceRemaining() > 0)
                    {
                        // Push out, keep going, consume one piercing hit
                        const sf::Vector2f pos = p.getPosition();
                        p.setPosition(pos.x + n.x * (pen + 0.5f), pos.y + n.y * (pen + 0.5f));
                        p.consumePierceHit();
                        break;
                    }

                    // Normal bounce
                    {
                        sf::Vector2f v = p.getVelocity();
                        v = reflect(v, n);
                        p.setVelocity(v);
                        const sf::Vector2f pos = p.getPosition();
                        p.setPosition(pos.x + n.x * (pen + 0.5f), pos.y + n.y * (pen + 0.5f));
                    }

                    break; // one collision per projectile per frame
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
        if (used >= budget && projectiles.empty())
        {
            state = State::Lose;
            loseReason = LoseReason::OutOfAmmo;
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
        drawDangerLine(target);

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
            drawOverlay(target, "DEFEAT", (loseReason == LoseReason::DangerLine) ? "Bricks reached the danger line" : "Out of ammo");
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

    enum class LoseReason
    {
        OutOfAmmo,
        DangerLine,
    };

    Cannon cannon;
    std::vector<Projectile> projectiles;
    std::vector<RebornGame::Brick> bricks;

    int score = 0;
    int budget = 50;
    int used = 0;
    int maxActive = 4;
    float fireCooldown = 0.0f;
    int combo = 0;

    bool firingHeld = false;
    State state = State::Playing;
    LoseReason loseReason = LoseReason::OutOfAmmo;
    Projectile::ShotType currentShot = Projectile::ShotType::Normal;

    float dangerLineY = WINDOW_H - 150.0f;

    Button btnResume;
    Button btnRestart;
    Button btnBack;

    void resetLevel()
    {
        bricks.clear();
        projectiles.clear();
        score = 0;
        used = 0;
        combo = 0;
        fireCooldown = 0.0f;
        firingHeld = false;
        state = State::Playing;
        loseReason = LoseReason::OutOfAmmo;
        currentShot = Projectile::ShotType::Normal;
        maxActive = maxActiveShots(ctx.settings.difficulty);
        dangerLineY = WINDOW_H - 150.0f;

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

    void fire(Projectile::ShotType type)
    {
        const float angle = cannon.getDirectionRadians();
        const sf::Vector2f pos = cannon.getPosition();
        const float speed = projectileSpeed(ctx.settings.difficulty);

        const float offset = 34.0f;
        const float ox = offset * std::cos(angle);
        const float oy = offset * std::sin(angle);

        projectiles.emplace_back(pos.x + ox, pos.y + oy, angle, WINDOW_W, WINDOW_H, speed, type);
    }

    void drawHud(sf::RenderTarget &target)
    {
        if (!ctx.assets.uiFontLoaded)
            return;

        std::ostringstream ss;
        ss << "Score: " << score
           << "    Ammo: " << (budget - used) << "/" << budget
           << "    Active: " << projectiles.size() << "/" << maxActive
           << "    Shot: " << shotName(currentShot)
           << "    Cooldown: " << (fireCooldown > 0.0f ? "..." : "READY")
           << "    Combo: x" << (combo > 0 ? combo : 0)
           << "    (Hold LMB to fire, 1/2/3 switch)";
        sf::Text t(ss.str(), ctx.assets.uiFont, 18);
        t.setFillColor(sf::Color(220, 220, 235));
        t.setPosition(12.0f, 10.0f);
        target.draw(t);
    }

    void drawDangerLine(sf::RenderTarget &target)
    {
        sf::RectangleShape line(sf::Vector2f(WINDOW_W, 2.0f));
        line.setPosition(0.0f, dangerLineY);
        line.setFillColor(sf::Color(255, 80, 80, 220));
        target.draw(line);

        // progress bar (how close the lowest brick is)
        float maxBottom = 0.0f;
        for (const auto &b : bricks)
        {
            if (b.isDestroyed())
                continue;
            const sf::Vector2f pos = b.getPosition();
            const sf::Vector2f sz = b.getSize();
            maxBottom = std::max(maxBottom, pos.y + sz.y);
        }
        const float p = clampf(maxBottom / dangerLineY, 0.0f, 1.0f);
        sf::RectangleShape barBg(sf::Vector2f(160.0f, 10.0f));
        barBg.setPosition(WINDOW_W - 180.0f, 14.0f);
        barBg.setFillColor(sf::Color(0, 0, 0, 140));
        barBg.setOutlineThickness(1.0f);
        barBg.setOutlineColor(sf::Color(255, 80, 80, 180));
        target.draw(barBg);
        sf::RectangleShape bar(sf::Vector2f(160.0f * p, 10.0f));
        bar.setPosition(WINDOW_W - 180.0f, 14.0f);
        bar.setFillColor(sf::Color(255, 80, 80, 200));
        target.draw(bar);
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
