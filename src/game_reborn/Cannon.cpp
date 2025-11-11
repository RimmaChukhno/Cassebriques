#include "Cannon.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Cannon::Cannon(float screenW, float screenH)
    : GameObject(screenW / 2.0f, screenH - 80.0f, 40.0f, 60.0f, sf::Color::White),
      screenWidth(screenW), screenHeight(screenH)
{
    // Origine au centre du bas pour la rotation
    // Note: GameObject ne gère pas l'origine, on devra le gérer dans draw()
}

void Cannon::pointAt(float mouseX, float mouseY)
{
    // Calculer l'angle vers la souris
    float dx = mouseX - position.x;
    float dy = mouseY - position.y;

    float angleRad = std::atan2(dy, dx);
    rotation = angleRad * 180.0f / static_cast<float>(M_PI);
}

float Cannon::getDirectionRadians() const
{
    return rotation * static_cast<float>(M_PI) / 180.0f;
}

void Cannon::draw(sf::RenderWindow &window)
{
    // Dessiner un rectangle pour le canon
    sf::RectangleShape rect(size);

    // Origine au centre du bas pour que la rotation soit autour de la base
    rect.setOrigin(size.x / 2.0f, size.y);

    // Position et rotation
    rect.setPosition(position);
    rect.setRotation(rotation);
    rect.setFillColor(color);

    window.draw(rect);
}
