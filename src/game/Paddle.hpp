#pragma once

#include "../core/GameObject.hpp"

/**
 * @brief Raquette du joueur (version classique)
 */
class Paddle : public GameObject
{
private:
    float speed;       // Vitesse de déplacement
    float screenWidth; // Largeur de l'écran (pour limiter le mouvement)

public:
    /**
     * @brief Constructeur
     * @param x Position X initiale
     * @param y Position Y (fixe)
     * @param width Largeur de la raquette
     * @param height Hauteur de la raquette
     * @param screenW Largeur de l'écran
     * @param spd Vitesse de déplacement
     */
    Paddle(float x, float y, float width, float height, float screenW, float spd = 400.0f);

    /**
     * @brief Met à jour la raquette
     * @param deltaTime Temps écoulé
     * @param moveLeft true si déplacement vers la gauche
     * @param moveRight true si déplacement vers la droite
     */
    void update(float deltaTime, bool moveLeft, bool moveRight);

    /**
     * @brief Calcule l'angle de rebond de la balle selon le point d'impact
     * @param ballX Position X de la balle
     * @return Angle en degrés (-60 à +60)
     */
    float calculateBounceAngle(float ballX) const;
};

