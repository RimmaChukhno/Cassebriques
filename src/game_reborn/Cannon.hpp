#pragma once

#include "../core/GameObject.hpp"

/**
 * @brief Canon du joueur (version Reborn)
 */
class Cannon : public GameObject
{
private:
    float screenWidth;  // Largeur de l'écran
    float screenHeight; // Hauteur de l'écran

public:
    /**
     * @brief Constructeur
     * @param screenW Largeur de l'écran
     * @param screenH Hauteur de l'écran
     */
    Cannon(float screenW, float screenH);

    /**
     * @brief Met à jour le canon pour pointer vers la souris
     * @param mouseX Position X de la souris
     * @param mouseY Position Y de la souris
     */
    void pointAt(float mouseX, float mouseY);

    /**
     * @brief Retourne la direction du canon en radians
     */
    float getDirectionRadians() const;

    /**
     * @brief Dessine le canon avec rotation correcte
     * @param window Fenêtre SFML où dessiner
     */
    void draw(sf::RenderWindow &window) override;
};

