#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "../core/InputManager.h"
#include "Cannon.h"
#include "Projectile.h"
#include "Brick.h"

/**
 * @brief Classe principale du jeu Casse-Briques Reborn
 */
class GameReborn
{
private:
    // Fenêtre SFML
    sf::RenderWindow window;
    sf::Clock clock;

    // Gestionnaire d'entrées
    InputManager inputManager;

    // Objets du jeu
    Cannon cannon;
    std::vector<Projectile> projectiles;
    std::vector<RebornGame::Brick> bricks;

    // État du jeu
    bool hasActiveProjectile;
    bool mouseButtonWasPressed; // Pour détecter un seul clic

    // Constantes
    static constexpr float WINDOW_WIDTH = 800.0f;
    static constexpr float WINDOW_HEIGHT = 600.0f;
    static constexpr int BRICK_ROWS = 5;
    static constexpr int BRICK_COLS = 10;

public:
    GameReborn();
    ~GameReborn() = default;

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
     * @brief Tire un projectile
     */
    void fireProjectile();
};
