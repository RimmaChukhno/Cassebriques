#include "Projectile.hpp"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace
{
    sf::Color colorForShot(Projectile::ShotType t)
    {
        switch (t)
        {
        case Projectile::ShotType::Piercing:
            return sf::Color(255, 80, 255); // magenta
        case Projectile::ShotType::Explosive:
            return sf::Color(255, 160, 40); // orange
        case Projectile::ShotType::Normal:
        default:
            return sf::Color::Cyan;
        }
    }
} // namespace

Projectile::Projectile(float x, float y, float angleRad, float screenW, float screenH, float speed, ShotType shotType)
    : GameObject(x, y, 8.0f, colorForShot(shotType)), screenWidth(screenW), screenHeight(screenH), type(shotType)
{

    // Initialiser la vitesse selon l'angle
    velocity.x = speed * std::cos(angleRad);
    velocity.y = speed * std::sin(angleRad);

    if (type == ShotType::Piercing)
    {
        pierceRemaining = 2;
    }
    else if (type == ShotType::Explosive)
    {
        explosionRadius = 60.0f;
    }
}

void Projectile::update(float deltaTime)
{
    if (dead)
        return;

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
    if (dead)
        return true;
    return position.y - radius > screenHeight;
}

Projectile::ShotType Projectile::getShotType() const
{
    return type;
}

int Projectile::getPierceRemaining() const
{
    return pierceRemaining;
}

void Projectile::consumePierceHit()
{
    if (pierceRemaining > 0)
        pierceRemaining--;
    if (pierceRemaining <= 0 && type == ShotType::Piercing)
    {
        // after piercing hits are used, it becomes a normal projectile
        type = ShotType::Normal;
        color = colorForShot(type);
    }
}

float Projectile::getExplosionRadius() const
{
    return explosionRadius;
}

bool Projectile::isDead() const
{
    return dead;
}

void Projectile::kill()
{
    dead = true;
}

bool Projectile::hasHitSomething() const
{
    return hitSomething;
}

void Projectile::markHit()
{
    hitSomething = true;
}
