#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "../core/InputManager.hpp"
#include "Paddle.hpp"
#include "Ball.hpp"
#include "Brick.hpp"

/**
 * @brief Classe principale du jeu Casse-Briques classique
 */
class Game
{
private:
    // Fenêtre SFML
    sf::RenderWindow window;
    sf::Clock clock;

    // Gestionnaire d'entrées
    InputManager inputManager;

    // Objets du jeu
    Paddle paddle;
    Ball ball;
    std::vector<ClassicGame::Brick> bricks;

    // État du jeu
    int score;
    bool gameOver;
    bool gameWon;
    bool ballActive;

    // Constantes
    static constexpr float WINDOW_WIDTH = 800.0f;
    static constexpr float WINDOW_HEIGHT = 600.0f;
    static constexpr float PADDLE_WIDTH = 100.0f;
    static constexpr float PADDLE_HEIGHT = 20.0f;
    static constexpr float BALL_RADIUS = 8.0f;
    static constexpr int BRICK_ROWS = 5;
    static constexpr int BRICK_COLS = 10;

    // Difficulté progressive
    float difficultyTimer;
    static constexpr float DIFFICULTY_INTERVAL = 10.0f; // Augmente toutes les 10 secondes

public:
    Game();
    ~Game() = default;

    /**
     * @brief Initialise le jeu
     */
    void init();

    /**
     * @brief Boucle principale du jeu
     */
    void run();

private:
    /**
     * @brief Met à jour la logique du jeu
     * @param deltaTime Temps écoulé
     */
    void update(float deltaTime);

    /**
     * @brief Dessine tout le jeu
     */
    void draw();

    /**
     * @brief Crée la grille de briques
     */
    void createBricks();

    /**
     * @brief Gère les collisions
     */
    void handleCollisions();

    /**
     * @brief Vérifie les conditions de victoire/défaite
     */
    void checkGameState();

    /**
     * @brief Affiche le texte du score
     */
    void drawScore();

    /**
     * @brief Affiche le message de fin de jeu
     */
    void drawGameOver();
};

