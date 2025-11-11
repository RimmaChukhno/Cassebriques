#include "GameObject.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

GameObject::GameObject(float x, float y, float width, float height, const sf::Color &col)
    : position(x, y), size(width, height), radius(0.0f), isCircle(false),
      velocity(0.0f, 0.0f), rotation(0.0f), color(col)
{
}

GameObject::GameObject(float x, float y, float radius, const sf::Color &col)
    : position(x, y), size(0.0f, 0.0f), radius(radius), isCircle(true),
      velocity(0.0f, 0.0f), rotation(0.0f), color(col)
{
}

void GameObject::update(float deltaTime)
{
    // Mise à jour de la position selon la vitesse
    position += velocity * deltaTime;
}

void GameObject::draw(sf::RenderWindow &window)
{
    if (isCircle)
    {
        // Dessiner un cercle
        sf::CircleShape circle(radius);
        circle.setPosition(position);
        circle.setOrigin(radius, radius); // Origine au centre
        circle.setFillColor(color);
        circle.setRotation(rotation);
        window.draw(circle);
    }
    else
    {
        // Dessiner un rectangle
        sf::RectangleShape rect(size);
        rect.setPosition(position);
        rect.setFillColor(color);
        rect.setRotation(rotation);
        window.draw(rect);
    }
}

AABB GameObject::getAABB() const
{
    AABB box;

    if (isCircle)
    {
        // Pour un cercle, la boîte englobante est un carré
        box.left = position.x - radius;
        box.top = position.y - radius;
        box.right = position.x + radius;
        box.bottom = position.y + radius;
    }
    else
    {
        // Pour un rectangle, prendre en compte la rotation si nécessaire
        // Version simplifiée : on ignore la rotation pour l'AABB
        box.left = position.x;
        box.top = position.y;
        box.right = position.x + size.x;
        box.bottom = position.y + size.y;
    }

    return box;
}

bool GameObject::checkCollision(const GameObject &other) const
{
    // Test rapide avec AABB
    AABB thisBox = getAABB();
    AABB otherBox = other.getAABB();

    if (!thisBox.intersects(otherBox))
    {
        return false;
    }

    // Si les deux sont des cercles, test précis
    if (isCircle && other.isCircle)
    {
        float dx = position.x - other.position.x;
        float dy = position.y - other.position.y;
        float distance = std::sqrt(dx * dx + dy * dy);
        return distance < (radius + other.radius);
    }

    // Si l'un est un cercle et l'autre un rectangle, test cercle-AABB
    if (isCircle && !other.isCircle)
    {
        // Trouver le point le plus proche du cercle sur le rectangle
        float closestX = std::max(otherBox.left, std::min(position.x, otherBox.right));
        float closestY = std::max(otherBox.top, std::min(position.y, otherBox.bottom));

        float dx = position.x - closestX;
        float dy = position.y - closestY;
        float distanceSquared = dx * dx + dy * dy;

        return distanceSquared < (radius * radius);
    }

    if (!isCircle && other.isCircle)
    {
        // Même logique mais inversée
        float closestX = std::max(thisBox.left, std::min(other.position.x, thisBox.right));
        float closestY = std::max(thisBox.top, std::min(other.position.y, thisBox.bottom));

        float dx = other.position.x - closestX;
        float dy = other.position.y - closestY;
        float distanceSquared = dx * dx + dy * dy;

        return distanceSquared < (other.radius * other.radius);
    }

    // Rectangle-Rectangle : l'intersection AABB suffit
    return true;
}

bool GameObject::isOutOfBounds(float screenWidth, float screenHeight) const
{
    AABB box = getAABB();

    // Vérifier si complètement hors de l'écran
    return box.right < 0 || box.left > screenWidth ||
           box.bottom < 0 || box.top > screenHeight;
}
