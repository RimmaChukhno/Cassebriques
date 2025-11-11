#pragma once

#include "../core/GameObject.h"

namespace RebornGame
{
    /**
     * @brief Brique avec points de vie (version Reborn)
     */
    class Brick : public GameObject
    {
    private:
        int maxHP;      // Points de vie maximum
        int currentHP;  // Points de vie actuels
        bool destroyed; // État de destruction

    public:
        /**
         * @brief Constructeur
         * @param x Position X
         * @param y Position Y
         * @param width Largeur
         * @param height Hauteur
         * @param hp Points de vie
         */
        Brick(float x, float y, float width, float height, int hp = 1);

        /**
         * @brief Vérifie si la brique est détruite
         */
        bool isDestroyed() const { return destroyed; }

        /**
         * @brief Inflige des dégâts à la brique
         * @param damage Dégâts infligés
         */
        void takeDamage(int damage = 1);

        /**
         * @brief Retourne les points de vie actuels
         */
        int getHP() const { return currentHP; }

        /**
         * @brief Retourne les points de vie maximum
         */
        int getMaxHP() const { return maxHP; }

        /**
         * @brief Met à jour la couleur selon les HP restants
         */
        void updateColor();
    };
} // namespace RebornGame
