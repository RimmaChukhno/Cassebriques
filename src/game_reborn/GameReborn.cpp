#include "GameReborn.h"
#include <iostream>
#include <algorithm>
#include <cmath>

GameReborn::GameReborn()
    : window(sf::VideoMode(static_cast<unsigned int>(WINDOW_WIDTH),
                           static_cast<unsigned int>(WINDOW_HEIGHT)),
             "Casse-Briques Reborn"),
      cannon(WINDOW_WIDTH, WINDOW_HEIGHT),
      hasActiveProjectile(false), mouseButtonWasPressed(false)
{

    window.setFramerateLimit(60);
}

void GameReborn::init()
{
    createBricks();
}

void GameReborn::run()
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

void GameReborn::update(float deltaTime)
{
    // Mettre à jour la position de la souris et pointer le canon
    sf::Vector2i mousePos = inputManager.getMousePosition();
    cannon.pointAt(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    // Gestion du tir (clic gauche) - détecter un seul clic
    bool mouseButtonPressed = inputManager.isMouseButtonPressed(sf::Mouse::Left);
    if (mouseButtonPressed && !mouseButtonWasPressed && !hasActiveProjectile)
    {
        fireProjectile();
        hasActiveProjectile = true;
    }
    mouseButtonWasPressed = mouseButtonPressed;

    // Mise à jour des projectiles
    for (auto &projectile : projectiles)
    {
        projectile.update(deltaTime);
    }

    // Supprimer les projectiles perdus
    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(),
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

    // Gestion des collisions
    handleCollisions();
}

void GameReborn::draw()
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

    // Dessiner les projectiles
    for (auto &projectile : projectiles)
    {
        projectile.draw(window);
    }

    // Dessiner le canon
    cannon.draw(window);

    window.display();
}

void GameReborn::createBricks()
{
    bricks.clear();

    float brickWidth = 70.0f;
    float brickHeight = 30.0f;
    float spacing = 5.0f;
    float startX = (WINDOW_WIDTH - (BRICK_COLS * (brickWidth + spacing) - spacing)) / 2.0f;
    float startY = 50.0f;

    // Créer des briques avec différents HP (1-3)
    for (int row = 0; row < BRICK_ROWS; row++)
    {
        for (int col = 0; col < BRICK_COLS; col++)
        {
            float x = startX + col * (brickWidth + spacing);
            float y = startY + row * (brickHeight + spacing);

            // HP selon la ligne (plus de HP pour les briques du haut)
            int hp = (BRICK_ROWS - row) / 2 + 1; // 1 à 3 HP
            hp = std::max(1, std::min(3, hp));

            RebornGame::Brick brick(x, y, brickWidth, brickHeight, hp);
            bricks.push_back(brick);
        }
    }
}

void GameReborn::handleCollisions()
{
    // Collisions projectile-briques
    for (auto &projectile : projectiles)
    {
        for (auto &brick : bricks)
        {
            if (!brick.isDestroyed() && projectile.checkCollision(brick))
            {
                // Inverser la direction du projectile
                sf::Vector2f vel = projectile.getVelocity();
                projectile.setVelocity(-vel.x, -vel.y);

                // Infliger des dégâts à la brique
                brick.takeDamage(1);
                break; // Une collision à la fois par projectile
            }
        }
    }
}

void GameReborn::fireProjectile()
{
    float angleRad = cannon.getDirectionRadians();
    sf::Vector2f cannonPos = cannon.getPosition();

    // Ajuster la position de départ (devant le canon)
    float offsetX = 30.0f * std::cos(angleRad);
    float offsetY = 30.0f * std::sin(angleRad);

    Projectile proj(cannonPos.x + offsetX,
                    cannonPos.y + offsetY,
                    angleRad,
                    WINDOW_WIDTH,
                    WINDOW_HEIGHT,
                    500.0f);
    projectiles.push_back(proj);
}
