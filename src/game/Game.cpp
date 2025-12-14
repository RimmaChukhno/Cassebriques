#include "Game.hpp"
#include <iostream>
#include <algorithm>

Game::Game()
    : window(sf::VideoMode(static_cast<unsigned int>(WINDOW_WIDTH),
                           static_cast<unsigned int>(WINDOW_HEIGHT)),
             "Casse-Briques"),
      paddle(WINDOW_WIDTH / 2.0f - PADDLE_WIDTH / 2.0f,
             WINDOW_HEIGHT - 50.0f,
             PADDLE_WIDTH, PADDLE_HEIGHT,
             WINDOW_WIDTH, 400.0f),
      ball(WINDOW_WIDTH / 2.0f,
           WINDOW_HEIGHT - 100.0f,
           BALL_RADIUS,
           WINDOW_WIDTH, WINDOW_HEIGHT,
           300.0f),
      score(0), gameOver(false), gameWon(false), ballActive(false), difficultyTimer(0.0f)
{

    window.setFramerateLimit(60);
}

void Game::init()
{
    createBricks();

    // Lancer la balle initiale
    ball.setVelocity(200.0f, -300.0f);
    ballActive = true;
}

void Game::run()
{
    init();

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            inputManager.processEvent(event);
        }

        inputManager.update(window);

        float deltaTime = clock.restart().asSeconds();
        update(deltaTime);

        draw();
    }
}

void Game::update(float deltaTime)
{
    if (gameOver || gameWon)
    {
        return;
    }

    // Mise à jour de la difficulté progressive
    difficultyTimer += deltaTime;
    if (difficultyTimer >= DIFFICULTY_INTERVAL)
    {
        ball.increaseSpeed(1.1f);
        difficultyTimer = 0.0f;
    }

    // Contrôle de la raquette
    bool moveLeft = inputManager.isKeyPressed(sf::Keyboard::Left) ||
                    inputManager.isKeyPressed(sf::Keyboard::A);
    bool moveRight = inputManager.isKeyPressed(sf::Keyboard::Right) ||
                     inputManager.isKeyPressed(sf::Keyboard::D);

    paddle.update(deltaTime, moveLeft, moveRight);

    // Mise à jour de la balle
    if (ballActive)
    {
        ball.update(deltaTime);

        // Vérifier si la balle est perdue
        if (ball.isLost())
        {
            gameOver = true;
            ballActive = false;
        }
    }

    // Gestion des collisions
    handleCollisions();

    // Vérifier l'état du jeu
    checkGameState();
}

void Game::draw()
{
    window.clear(sf::Color::Black);

    // Dessiner les briques
    for (auto &brick : bricks)
    {
        if (!brick.isDestroyed())
        {
            brick.draw(window);
        }
    }

    // Dessiner la raquette
    paddle.draw(window);

    // Dessiner la balle
    if (ballActive)
    {
        ball.draw(window);
    }

    // Dessiner le score
    drawScore();

    // Dessiner le message de fin
    if (gameOver || gameWon)
    {
        drawGameOver();
    }

    window.display();
}

void Game::createBricks()
{
    bricks.clear();

    float brickWidth = 70.0f;
    float brickHeight = 30.0f;
    float spacing = 5.0f;
    float startX = (WINDOW_WIDTH - (BRICK_COLS * (brickWidth + spacing) - spacing)) / 2.0f;
    float startY = 50.0f;

    // Palette de couleurs
    sf::Color colors[] = {
        sf::Color::Red,
        sf::Color(255, 165, 0), // Orange (RGB)
        sf::Color::Yellow,
        sf::Color::Green,
        sf::Color::Cyan};

    for (int row = 0; row < BRICK_ROWS; row++)
    {
        for (int col = 0; col < BRICK_COLS; col++)
        {
            float x = startX + col * (brickWidth + spacing);
            float y = startY + row * (brickHeight + spacing);

            int points = (BRICK_ROWS - row) * 10; // Plus de points pour les briques du haut
            ClassicGame::Brick brick(x, y, brickWidth, brickHeight, colors[row % 5], points);
            bricks.push_back(brick);
        }
    }
}

void Game::handleCollisions()
{
    if (!ballActive)
        return;

    // Collision balle-raquette
    if (ball.checkCollision(paddle))
    {
        ball.bounceOnPaddle(paddle.getPosition().x, PADDLE_WIDTH);
    }

    // Collisions balle-briques
    for (auto &brick : bricks)
    {
        if (!brick.isDestroyed() && ball.checkCollision(brick))
        {
            // Inverser la direction de la balle
            sf::Vector2f vel = ball.getVelocity();
            ball.setVelocity(-vel.x, -vel.y);

            // Détruire la brique et ajouter les points
            score += brick.getPoints();
            brick.destroy();
            break; // Une collision à la fois
        }
    }
}

void Game::checkGameState()
{
    // Vérifier la victoire (toutes les briques détruites)
    bool allDestroyed = true;
    for (const auto &brick : bricks)
    {
        if (!brick.isDestroyed())
        {
            allDestroyed = false;
            break;
        }
    }

    if (allDestroyed)
    {
        gameWon = true;
    }
}

void Game::drawScore()
{
    // Note: Pour un affichage complet, il faudrait charger une police
    // Ici, on affiche juste dans la console pour l'instant
    // TODO: Implémenter l'affichage avec sf::Text
}

void Game::drawGameOver()
{
    // Note: Pour un affichage complet, il faudrait charger une police
    // TODO: Implémenter l'affichage avec sf::Text
    std::cout << (gameWon ? "Victoire !" : "Game Over !") << std::endl;
}
