#pragma once

/**
 * @brief Structure représentant une boîte englobante alignée sur les axes (AABB)
 */
struct AABB
{
    float left;   // Coordonnée X minimale
    float top;    // Coordonnée Y minimale
    float right;  // Coordonnée X maximale
    float bottom; // Coordonnée Y maximale

    /**
     * @brief Vérifie si cette AABB intersecte avec une autre
     * @param other L'autre AABB à tester
     * @return true si intersection, false sinon
     */
    bool intersects(const AABB &other) const
    {
        return !(right < other.left || left > other.right ||
                 bottom < other.top || top > other.bottom);
    }

    /**
     * @brief Calcule la largeur de la boîte
     */
    float getWidth() const { return right - left; }

    /**
     * @brief Calcule la hauteur de la boîte
     */
    float getHeight() const { return bottom - top; }
};
