#include "Paddle.hpp"
#include <algorithm>

Paddle::Paddle(float x, float y, float width, float height, float screenW, float spd)
    : GameObject(x, y, width, height, sf::Color::White), speed(spd), screenWidth(screenW)
{
}

void Paddle::update(float deltaTime, bool moveLeft, bool moveRight)
{
    float moveX = 0.0f;

    if (moveLeft)
    {
        moveX = -speed * deltaTime;
    }
    if (moveRight)
    {
        moveX = speed * deltaTime;
    }

    // Mettre à jour la position
    float newX = position.x + moveX;

    // Limiter aux bords de l'écran
    newX = std::max(0.0f, std::min(newX, screenWidth - size.x));

    position.x = newX;
}

float Paddle::calculateBounceAngle(float ballX) const
{
    // Calculer le point d'impact relatif (0.0 = gauche, 1.0 = droite)
    float hitPos = (ballX - position.x) / size.x;

    // Normaliser entre -1 et 1
    hitPos = std::max(0.0f, std::min(1.0f, hitPos));
    hitPos = (hitPos - 0.5f) * 2.0f; // -1 à 1

    // Angle entre -60° et +60°
    return hitPos * 60.0f;
}
