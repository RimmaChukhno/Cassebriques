#include "Projectile.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Projectile::Projectile(float x, float y, float angleRad, float screenW, float screenH, float speed)
    : GameObject(x, y, 8.0f, sf::Color::Cyan), screenWidth(screenW), screenHeight(screenH)
{

    // Initialiser la vitesse selon l'angle
    velocity.x = speed * std::cos(angleRad);
    velocity.y = speed * std::sin(angleRad);
}

void Projectile::update(float deltaTime)
{
    // Mise à jour de la position
    GameObject::update(deltaTime);

    // Rebonds sur les murs gauche et droit
    if (position.x - radius < 0)
    {
        position.x = radius;
        velocity.x = -velocity.x;
    }
    else if (position.x + radius > screenWidth)
    {
        position.x = screenWidth - radius;
        velocity.x = -velocity.x;
    }

    // Rebond sur le mur du haut
    if (position.y - radius < 0)
    {
        position.y = radius;
        velocity.y = -velocity.y;
    }

    // Pas de rebond sur le bas (le projectile est perdu)
}

bool Projectile::isLost() const
{
    return position.y - radius > screenHeight;
}
