#pragma once

#include "../core/GameObject.h"

/**
 * @brief Projectile tiré par le canon (version Reborn)
 */
class Projectile : public GameObject
{
private:
    float screenWidth;  // Largeur de l'écran
    float screenHeight; // Hauteur de l'écran

public:
    /**
     * @brief Constructeur
     * @param x Position X initiale (canon)
     * @param y Position Y initiale (canon)
     * @param angleRad Angle de tir en radians
     * @param screenW Largeur de l'écran
     * @param screenH Hauteur de l'écran
     * @param speed Vitesse du projectile
     */
    Projectile(float x, float y, float angleRad, float screenW, float screenH, float speed = 500.0f);

    /**
     * @brief Met à jour le projectile avec rebonds sur les murs
     * @param deltaTime Temps écoulé
     */
    void update(float deltaTime) override;

    /**
     * @brief Vérifie si le projectile est perdu (sorti par le bas)
     */
    bool isLost() const;
};
