#pragma once

#include "../core/GameObject.hpp"

/**
 * @brief Balle du jeu (version classique)
 */
class Ball : public GameObject
{
private:
    float baseSpeed;    // Vitesse de base
    float screenWidth;  // Largeur de l'écran
    float screenHeight; // Hauteur de l'écran

public:
    /**
     * @brief Constructeur
     * @param x Position X initiale
     * @param y Position Y initiale
     * @param radius Rayon de la balle
     * @param screenW Largeur de l'écran
     * @param screenH Hauteur de l'écran
     * @param spd Vitesse initiale
     */
    Ball(float x, float y, float radius, float screenW, float screenH, float spd = 300.0f);

    /**
     * @brief Met à jour la balle avec rebonds sur les murs
     * @param deltaTime Temps écoulé
     */
    void update(float deltaTime) override;

    /**
     * @brief Fait rebondir la balle sur la raquette
     * @param paddleX Position X de la raquette
     * @param paddleWidth Largeur de la raquette
     */
    void bounceOnPaddle(float paddleX, float paddleWidth);

    /**
     * @brief Vérifie si la balle est perdue (sortie par le bas)
     */
    bool isLost() const;

    /**
     * @brief Augmente la vitesse (pour la difficulté progressive)
     * @param multiplier Multiplicateur de vitesse
     */
    void increaseSpeed(float multiplier = 1.1f);
};

