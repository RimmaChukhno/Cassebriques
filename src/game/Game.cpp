#include "Game.hpp"
#include <iostream>
#include <algorithm>
#include <sstream>

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
      score(0), lives(3), gameOver(false), gameWon(false), ballActive(false), difficultyTimer(0.0f)
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
            lives--;
            ballActive = false;
            
            if (lives <= 0)
            {
                gameOver = true;
            }
            else
            {
                // Relancer la balle après un court délai
                ball.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT - 100.0f);
                ball.setVelocity(200.0f, -300.0f);
                ballActive = true;
            }
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
    // Afficher le score
    std::ostringstream scoreText;
    scoreText << "Score: " << score;
    sf::Text scoreLabel(scoreText.str(), font, 24);
    scoreLabel.setPosition(10.0f, 10.0f);
    scoreLabel.setFillColor(sf::Color::White);
    window.draw(scoreLabel);
    
    // Afficher les vies
    std::ostringstream livesText;
    livesText << "Vies: " << lives;
    sf::Text livesLabel(livesText.str(), font, 24);
    livesLabel.setPosition(10.0f, 40.0f);
    livesLabel.setFillColor(sf::Color::White);
    window.draw(livesLabel);
}

void Game::drawGameOver()
{
    if (gameWon)
    {
        drawVictoryScreen();
    }
    else
    {
        drawDefeatScreen();
    }
}

void Game::drawVictoryScreen()
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
    message << "Vies restantes: " << lives << "\n\n";
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

void Game::drawDefeatScreen()
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
    message << "Vous avez perdu toutes vos vies !\n\n";
    message << "Score final: " << score << "\n";
    
    // Compter les briques détruites
    int destroyedCount = 0;
    for (const auto &brick : bricks)
    {
        if (brick.isDestroyed())
        {
            destroyedCount++;
        }
    }
    message << "Briques detruites: " << destroyedCount << "/" << bricks.size() << "\n\n";
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
