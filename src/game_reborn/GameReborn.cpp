#include "GameReborn.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <sstream>

GameReborn::GameReborn()
    : window(sf::VideoMode(static_cast<unsigned int>(WINDOW_WIDTH),
                           static_cast<unsigned int>(WINDOW_HEIGHT)),
             "Casse-Briques Reborn"),
      cannon(WINDOW_WIDTH, WINDOW_HEIGHT),
      score(0), maxProjectiles(50), projectilesUsed(0),
      gameOver(false), gameWon(false),
      hasActiveProjectile(false), mouseButtonWasPressed(false)
{
    // Charger la police (utilise une police système par défaut si la police personnalisée n'existe pas)
    bool fontLoaded = false;
    if (!fontLoaded)
    {
        fontLoaded = font.loadFromFile("assets/fonts/arial.ttf");
    }
    #ifdef _WIN32
    if (!fontLoaded)
    {
        fontLoaded = font.loadFromFile("C:/Windows/Fonts/arial.ttf");
    }
    if (!fontLoaded)
    {
        fontLoaded = font.loadFromFile("C:/Windows/Fonts/calibri.ttf");
    }
    #endif
    // Si aucune police n'est chargée, le texte ne s'affichera pas mais le jeu continuera
    
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
            
            // Permettre de quitter avec ECHAP quand le jeu est terminé
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Escape && (gameOver || gameWon))
                {
                    window.close();
                }
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
    if (gameOver || gameWon)
    {
        return;
    }
    
    // Mettre à jour la position de la souris et pointer le canon
    sf::Vector2i mousePos = inputManager.getMousePosition();
    cannon.pointAt(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    // Gestion du tir (clic gauche) - détecter un seul clic
    bool mouseButtonPressed = inputManager.isMouseButtonPressed(sf::Mouse::Left);
    if (mouseButtonPressed && !mouseButtonWasPressed && !hasActiveProjectile && projectilesUsed < maxProjectiles)
    {
        fireProjectile();
        hasActiveProjectile = true;
        projectilesUsed++;
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
    
    // Vérifier l'état du jeu
    checkGameState();
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
    
    // Dessiner le score
    drawScore();
    
    // Dessiner les fenêtres de fin de jeu
    if (gameWon)
    {
        drawVictoryScreen();
    }
    else if (gameOver)
    {
        drawDefeatScreen();
    }

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
                
                // Ajouter des points selon les HP de la brique
                if (brick.isDestroyed())
                {
                    score += brick.getMaxHP() * 10;
                }
                else
                {
                    score += 5; // Points pour avoir touché la brique
                }
                
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

void GameReborn::checkGameState()
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
        return;
    }
    
    // Vérifier la défaite (plus de projectiles disponibles et aucun projectile actif)
    if (projectilesUsed >= maxProjectiles && !hasActiveProjectile && projectiles.empty())
    {
        gameOver = true;
    }
}

void GameReborn::drawScore()
{
    // Afficher le score
    std::ostringstream scoreText;
    scoreText << "Score: " << score;
    sf::Text scoreLabel(scoreText.str(), font, 24);
    scoreLabel.setPosition(10.0f, 10.0f);
    scoreLabel.setFillColor(sf::Color::White);
    window.draw(scoreLabel);
    
    // Afficher les projectiles restants
    std::ostringstream projectilesText;
    projectilesText << "Projectiles: " << (maxProjectiles - projectilesUsed) << "/" << maxProjectiles;
    sf::Text projectilesLabel(projectilesText.str(), font, 24);
    projectilesLabel.setPosition(10.0f, 40.0f);
    projectilesLabel.setFillColor(sf::Color::White);
    window.draw(projectilesLabel);
}

void GameReborn::drawVictoryScreen()
{
    // Créer un rectangle semi-transparent pour le fond
    sf::RectangleShape overlay(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    overlay.setFillColor(sf::Color(0, 0, 0, 200));
    window.draw(overlay);
    
    // Fenêtre de victoire
    float winWidth = 500.0f;
    float winHeight = 350.0f;
    float winX = (WINDOW_WIDTH - winWidth) / 2.0f;
    float winY = (WINDOW_HEIGHT - winHeight) / 2.0f;
    
    sf::RectangleShape victoryWindow(sf::Vector2f(winWidth, winHeight));
    victoryWindow.setPosition(winX, winY);
    victoryWindow.setFillColor(sf::Color(50, 150, 50, 240));
    victoryWindow.setOutlineColor(sf::Color::Green);
    victoryWindow.setOutlineThickness(5.0f);
    window.draw(victoryWindow);
    
    // Titre "VICTOIRE !"
    sf::Text titleText("VICTOIRE !", font, 64);
    titleText.setFillColor(sf::Color::Green);
    titleText.setStyle(sf::Text::Bold);
    
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin(titleBounds.left + titleBounds.width / 2.0f,
                        titleBounds.top + titleBounds.height / 2.0f);
    titleText.setPosition(WINDOW_WIDTH / 2.0f, winY + 60.0f);
    window.draw(titleText);
    
    // Message de félicitations
    std::ostringstream message;
    message << "Felicitations !\nVous avez detruit toutes les briques !\n\n";
    message << "Score final: " << score << "\n";
    message << "Projectiles utilises: " << projectilesUsed << "/" << maxProjectiles << "\n\n";
    message << "Appuyez sur ECHAP pour quitter";
    
    sf::Text messageText(message.str(), font, 24);
    messageText.setFillColor(sf::Color::White);
    messageText.setLineSpacing(1.5f);
    
    sf::FloatRect messageBounds = messageText.getLocalBounds();
    messageText.setOrigin(messageBounds.left + messageBounds.width / 2.0f,
                          messageBounds.top + messageBounds.height / 2.0f);
    messageText.setPosition(WINDOW_WIDTH / 2.0f, winY + 180.0f);
    window.draw(messageText);
}

void GameReborn::drawDefeatScreen()
{
    // Créer un rectangle semi-transparent pour le fond
    sf::RectangleShape overlay(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    overlay.setFillColor(sf::Color(0, 0, 0, 200));
    window.draw(overlay);
    
    // Fenêtre de défaite
    float winWidth = 500.0f;
    float winHeight = 350.0f;
    float winX = (WINDOW_WIDTH - winWidth) / 2.0f;
    float winY = (WINDOW_HEIGHT - winHeight) / 2.0f;
    
    sf::RectangleShape defeatWindow(sf::Vector2f(winWidth, winHeight));
    defeatWindow.setPosition(winX, winY);
    defeatWindow.setFillColor(sf::Color(150, 50, 50, 240));
    defeatWindow.setOutlineColor(sf::Color::Red);
    defeatWindow.setOutlineThickness(5.0f);
    window.draw(defeatWindow);
    
    // Titre "DEFAITE"
    sf::Text titleText("DEFAITE", font, 64);
    titleText.setFillColor(sf::Color::Red);
    titleText.setStyle(sf::Text::Bold);
    
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin(titleBounds.left + titleBounds.width / 2.0f,
                        titleBounds.top + titleBounds.height / 2.0f);
    titleText.setPosition(WINDOW_WIDTH / 2.0f, winY + 60.0f);
    window.draw(titleText);
    
    // Message de défaite
    std::ostringstream message;
    message << "Vous avez epuise tous vos projectiles !\n\n";
    message << "Score final: " << score << "\n";
    message << "Briques detruites: ";
    
    // Compter les briques détruites
    int destroyedCount = 0;
    for (const auto &brick : bricks)
    {
        if (brick.isDestroyed())
        {
            destroyedCount++;
        }
    }
    message << destroyedCount << "/" << bricks.size() << "\n\n";
    message << "Appuyez sur ECHAP pour quitter";
    
    sf::Text messageText(message.str(), font, 24);
    messageText.setFillColor(sf::Color::White);
    messageText.setLineSpacing(1.5f);
    
    sf::FloatRect messageBounds = messageText.getLocalBounds();
    messageText.setOrigin(messageBounds.left + messageBounds.width / 2.0f,
                          messageBounds.top + messageBounds.height / 2.0f);
    messageText.setPosition(WINDOW_WIDTH / 2.0f, winY + 180.0f);
    window.draw(messageText);
}
