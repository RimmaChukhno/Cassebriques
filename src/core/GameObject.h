#pragma once

#include <SFML/Graphics.hpp>
#include "AABB.h"

/**
 * @brief Classe de base pour tous les objets du jeu
 *
 * Gère la position, la taille, la vitesse, la rotation et les collisions
 */
class GameObject
{
protected:
    sf::Vector2f position; // Position (x, y)
    sf::Vector2f size;     // Taille (width, height) - pour rectangle
    float radius;          // Rayon - pour cercle
    bool isCircle;         // true si forme circulaire, false si rectangle
    sf::Vector2f velocity; // Vitesse (vx, vy)
    float rotation;        // Angle de rotation en degrés
    sf::Color color;       // Couleur de l'objet

public:
    /**
     * @brief Constructeur pour un rectangle
     */
    GameObject(float x, float y, float width, float height, const sf::Color &col = sf::Color::White);

    /**
     * @brief Constructeur pour un cercle
     */
    GameObject(float x, float y, float radius, const sf::Color &col = sf::Color::White);

    virtual ~GameObject() = default;

    // Getters
    sf::Vector2f getPosition() const { return position; }
    sf::Vector2f getSize() const { return size; }
    float getRadius() const { return radius; }
    bool getIsCircle() const { return isCircle; }
    sf::Vector2f getVelocity() const { return velocity; }
    float getRotation() const { return rotation; }
    sf::Color getColor() const { return color; }

    // Setters
    void setPosition(const sf::Vector2f &pos) { position = pos; }
    void setPosition(float x, float y) { position = sf::Vector2f(x, y); }
    void setVelocity(const sf::Vector2f &vel) { velocity = vel; }
    void setVelocity(float vx, float vy) { velocity = sf::Vector2f(vx, vy); }
    void setRotation(float rot) { rotation = rot; }
    void setColor(const sf::Color &col) { color = col; }

    /**
     * @brief Met à jour l'objet (position, etc.)
     * @param deltaTime Temps écoulé depuis la dernière frame (en secondes)
     */
    virtual void update(float deltaTime);

    /**
     * @brief Dessine l'objet dans la fenêtre
     * @param window Fenêtre SFML où dessiner
     */
    virtual void draw(sf::RenderWindow &window);

    /**
     * @brief Calcule et retourne la zone de collision AABB
     */
    AABB getAABB() const;

    /**
     * @brief Vérifie la collision avec un autre GameObject
     * @param other L'autre GameObject à tester
     * @return true si collision, false sinon
     */
    bool checkCollision(const GameObject &other) const;

    /**
     * @brief Vérifie si l'objet est hors des limites de l'écran
     * @param screenWidth Largeur de l'écran
     * @param screenHeight Hauteur de l'écran
     * @return true si hors limites, false sinon
     */
    bool isOutOfBounds(float screenWidth, float screenHeight) const;
};
