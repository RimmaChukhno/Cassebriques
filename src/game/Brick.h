#pragma once

#include "../core/GameObject.h"

namespace ClassicGame
{
    /**
     * @brief Brique destructible (version classique)
     */
    class Brick : public GameObject
    {
    private:
        int points;     // Points donnés quand détruite
        bool destroyed; // État de destruction

    public:
        /**
         * @brief Constructeur
         * @param x Position X
         * @param y Position Y
         * @param width Largeur
         * @param height Hauteur
         * @param col Couleur
         * @param pts Points donnés
         */
        Brick(float x, float y, float width, float height, const sf::Color &col, int pts = 10);

        /**
         * @brief Vérifie si la brique est détruite
         */
        bool isDestroyed() const { return destroyed; }

        /**
         * @brief Détruit la brique
         */
        void destroy() { destroyed = true; }

        /**
         * @brief Retourne les points de la brique
         */
        int getPoints() const { return points; }
    };
} // namespace ClassicGame
