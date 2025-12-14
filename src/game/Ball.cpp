#include "Ball.hpp"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Ball::Ball(float x, float y, float radius, float screenW, float screenH, float spd)
    : GameObject(x, y, radius, sf::Color::White), baseSpeed(spd),
      screenWidth(screenW), screenHeight(screenH)
{
}

void Ball::update(float deltaTime)
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

    // Pas de rebond sur le bas (la balle est perdue)
}

void Ball::bounceOnPaddle(float paddleX, float paddleWidth)
{
    // Calculer le point d'impact relatif
    float hitPos = (position.x - paddleX) / paddleWidth;
    hitPos = std::max(0.0f, std::min(1.0f, hitPos));
    hitPos = (hitPos - 0.5f) * 2.0f; // -1 à 1

    // Angle entre -60° et +60°
    float angle = hitPos * 60.0f;
    float angleRad = angle * static_cast<float>(M_PI) / 180.0f;

    // Calculer la nouvelle vitesse (norme constante)
    float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    velocity.x = speed * std::sin(angleRad);
    velocity.y = -std::abs(speed * std::cos(angleRad)); // Toujours vers le haut
}

bool Ball::isLost() const
{
    return position.y - radius > screenHeight;
}

void Ball::increaseSpeed(float multiplier)
{
    float currentSpeed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    float newSpeed = currentSpeed * multiplier;

    // Normaliser la direction et appliquer la nouvelle vitesse
    if (currentSpeed > 0)
    {
        float dirX = velocity.x / currentSpeed;
        float dirY = velocity.y / currentSpeed;
        velocity.x = dirX * newSpeed;
        velocity.y = dirY * newSpeed;
    }
}
