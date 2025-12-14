#pragma once

#include <SFML/Graphics.hpp>
#include "AABB.hpp"

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
    sf::Vector2f getPosition() const;
    sf::Vector2f getSize() const;
    float getRadius() const;
    bool getIsCircle() const;
    sf::Vector2f getVelocity() const;
    float getRotation() const;
    sf::Color getColor() const;

    // Setters
    void setPosition(const sf::Vector2f &pos);
    void setPosition(float x, float y);
    void setVelocity(const sf::Vector2f &vel);
    void setVelocity(float vx, float vy);
    void setRotation(float rot);
    void setColor(const sf::Color &col);

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

