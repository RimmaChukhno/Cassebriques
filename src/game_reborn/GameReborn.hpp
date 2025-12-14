#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "../core/InputManager.hpp"
#include "Cannon.hpp"
#include "Projectile.hpp"
#include "Brick.hpp"

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
    int score;
    int maxProjectiles;        // Nombre maximum de projectiles autorisés
    int projectilesUsed;       // Nombre de projectiles utilisés
    bool gameOver;
    bool gameWon;
    bool hasActiveProjectile;
    bool mouseButtonWasPressed; // Pour détecter un seul clic
    
    // Police pour l'affichage du texte
    sf::Font font;

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
    
    /**
     * @brief Vérifie les conditions de victoire/défaite
     */
    void checkGameState();
    
    /**
     * @brief Affiche le texte du score
     */
    void drawScore();
    
    /**
     * @brief Affiche la fenêtre de victoire
     */
    void drawVictoryScreen();
    
    /**
     * @brief Affiche la fenêtre de défaite
     */
    void drawDefeatScreen();
};

