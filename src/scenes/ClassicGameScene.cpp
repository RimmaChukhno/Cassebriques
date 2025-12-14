#include "../app/App.hpp"
#include "../ui/Button.hpp"

#include "../game/Ball.hpp"
#include "../game/Paddle.hpp"
#include "../game/Brick.hpp"

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

constexpr float PADDLE_W = 110.0f;
constexpr float PADDLE_H = 20.0f;
constexpr float BALL_R = 8.0f;

constexpr int BRICK_ROWS = 6;
constexpr int BRICK_COLS = 10;

int startingLives(Difficulty d)
{
    switch (d)
    {
    case Difficulty::Easy:
        return 5;
    case Difficulty::Hard:
        return 2;
    case Difficulty::Normal:
    default:
        return 3;
    }
}

float baseBallSpeed(Difficulty d)
{
    switch (d)
    {
    case Difficulty::Easy:
        return 260.0f;
    case Difficulty::Hard:
        return 360.0f;
    case Difficulty::Normal:
    default:
        return 300.0f;
    }
}

float difficultyRamp(Difficulty d)
{
    switch (d)
    {
    case Difficulty::Easy:
        return 1.06f;
    case Difficulty::Hard:
        return 1.14f;
    case Difficulty::Normal:
    default:
        return 1.10f;
    }
}

void reflectBallOnAABB(Ball &ball, const AABB &box)
{
    const sf::Vector2f c = ball.getPosition();
    const float r = ball.getRadius();

    const float boxCx = (box.left + box.right) * 0.5f;
    const float boxCy = (box.top + box.bottom) * 0.5f;
    const float halfW = (box.right - box.left) * 0.5f;
    const float halfH = (box.bottom - box.top) * 0.5f;

    const float dx = c.x - boxCx;
    const float dy = c.y - boxCy;

    const float px = (halfW + r) - std::abs(dx);
    const float py = (halfH + r) - std::abs(dy);

    sf::Vector2f v = ball.getVelocity();
    if (px < py)
    {
        // Hit left/right
        v.x = -v.x;
        // Push out slightly to avoid sticking
        ball.setPosition(c.x + (dx < 0 ? -px : px), c.y);
    }
    else
    {
        // Hit top/bottom
        v.y = -v.y;
        ball.setPosition(c.x, c.y + (dy < 0 ? -py : py));
    }
    ball.setVelocity(v);
}

} // namespace

class ClassicGameScene final : public IScene
{
public:
    explicit ClassicGameScene(AppContext &ctx)
        : IScene(ctx),
          paddle(WINDOW_W / 2.0f - PADDLE_W / 2.0f, WINDOW_H - 60.0f, PADDLE_W, PADDLE_H, WINDOW_W, 520.0f),
          ball(WINDOW_W / 2.0f, WINDOW_H - 100.0f, BALL_R, WINDOW_W, WINDOW_H, baseBallSpeed(ctx.settings.difficulty))
    {
        lives = startingLives(ctx.settings.difficulty);
        resetLevel();

        // Overlay buttons use shared font
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
                if (state == State::Playing && !ballLaunched)
                    launchBall();
                else if (state == State::Win || state == State::Lose)
                    resetAll();
            }
        }

        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (event.mouseButton.button == sf::Mouse::Left && state == State::Playing && !ballLaunched)
                launchBall();
        }
    }

    void update(float dt) override
    {
        // Mouse interaction for overlays
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

        // Movement
        const bool left = sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A);
        const bool right = sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D);
        paddle.update(dt, left, right);

        // Ball behavior
        if (!ballLaunched)
        {
            // Stick ball on paddle
            const sf::Vector2f p = paddle.getPosition();
            ball.setPosition(p.x + PADDLE_W / 2.0f, p.y - BALL_R - 1.0f);
            return;
        }

        // Difficulty ramp over time (classic feel)
        rampTimer += dt;
        if (rampTimer >= 10.0f)
        {
            ball.increaseSpeed(difficultyRamp(ctx.settings.difficulty));
            rampTimer = 0.0f;
        }

        ball.update(dt);

        // Paddle collision: nudge out to avoid sticking
        if (ball.checkCollision(paddle))
        {
            ball.bounceOnPaddle(paddle.getPosition().x, PADDLE_W);
            ball.setPosition(ball.getPosition().x, paddle.getPosition().y - BALL_R - 1.0f);
        }

        // Brick collisions: reflect properly by contact side
        for (auto &b : bricks)
        {
            if (b.isDestroyed())
                continue;
            if (ball.checkCollision(b))
            {
                reflectBallOnAABB(ball, b.getAABB());
                score += b.getPoints();
                b.destroy();
                break;
            }
        }

        // Lose a life
        if (ball.isLost())
        {
            lives--;
            ballLaunched = false;

            if (lives <= 0)
            {
                state = State::Lose;
            }
        }

        // Win condition
        if (std::all_of(bricks.begin(), bricks.end(), [](const ClassicGame::Brick &b)
                        { return b.isDestroyed(); }))
        {
            state = State::Win;
        }
    }

    void render(sf::RenderTarget &target) override
    {
        // Background
        sf::RectangleShape bg(sf::Vector2f(WINDOW_W, WINDOW_H));
        bg.setFillColor(sf::Color(10, 10, 18));
        target.draw(bg);

        // Bricks
        for (auto &b : bricks)
            if (!b.isDestroyed())
                b.draw(static_cast<sf::RenderWindow &>(target));

        // Paddle + ball
        paddle.draw(static_cast<sf::RenderWindow &>(target));
        ball.draw(static_cast<sf::RenderWindow &>(target));

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
            drawOverlay(target, "DEFEAT", "Space: Restart");
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

    Paddle paddle;
    Ball ball;
    std::vector<ClassicGame::Brick> bricks;

    int score = 0;
    int lives = 3;
    bool ballLaunched = false;
    float rampTimer = 0.0f;
    State state = State::Playing;

    Button btnResume;
    Button btnRestart;
    Button btnBack;

    void resetLevel()
    {
        bricks.clear();
        score = 0;
        rampTimer = 0.0f;
        ballLaunched = false;
        state = State::Playing;

        const float brickW = 72.0f;
        const float brickH = 28.0f;
        const float spacing = 6.0f;
        const float totalW = BRICK_COLS * brickW + (BRICK_COLS - 1) * spacing;
        const float startX = (WINDOW_W - totalW) / 2.0f;
        const float startY = 80.0f;

        sf::Color colors[] = {
            sf::Color(255, 90, 90),
            sf::Color(255, 170, 70),
            sf::Color(255, 230, 90),
            sf::Color(110, 220, 140),
            sf::Color(90, 190, 255),
            sf::Color(180, 140, 255),
        };

        for (int r = 0; r < BRICK_ROWS; r++)
        {
            for (int c = 0; c < BRICK_COLS; c++)
            {
                const float x = startX + c * (brickW + spacing);
                const float y = startY + r * (brickH + spacing);
                const int pts = (BRICK_ROWS - r) * 10;
                bricks.emplace_back(x, y, brickW, brickH, colors[r % 6], pts);
            }
        }
    }

    void resetAll()
    {
        lives = startingLives(ctx.settings.difficulty);
        resetLevel();
    }

    void launchBall()
    {
        ballLaunched = true;
        const float spd = baseBallSpeed(ctx.settings.difficulty);
        ball.setVelocity(spd * 0.55f, -spd);
    }

    void drawHud(sf::RenderTarget &target)
    {
        if (!ctx.assets.uiFontLoaded)
            return;

        std::ostringstream ss;
        ss << "Score: " << score << "    Lives: " << lives << (ballLaunched ? "" : "    (Space to launch)");

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

std::unique_ptr<IScene> makeClassicGameScene(AppContext &ctx)
{
    return std::make_unique<ClassicGameScene>(ctx);
}


