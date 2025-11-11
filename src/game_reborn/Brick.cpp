#include "Brick.h"

namespace RebornGame
{

    Brick::Brick(float x, float y, float width, float height, int hp)
        : GameObject(x, y, width, height, sf::Color::Red),
          maxHP(hp), currentHP(hp), destroyed(false)
    {
        updateColor();
    }

    void Brick::takeDamage(int damage)
    {
        currentHP -= damage;

        if (currentHP <= 0)
        {
            currentHP = 0;
            destroyed = true;
        }
        else
        {
            updateColor();
        }
    }

    void Brick::updateColor()
    {
        // Changer la couleur selon les HP restants
        float hpRatio = static_cast<float>(currentHP) / static_cast<float>(maxHP);

        if (hpRatio > 0.66f)
        {
            color = sf::Color::Red; // 3 HP ou plus
        }
        else if (hpRatio > 0.33f)
        {
            color = sf::Color::Yellow; // 2 HP
        }
        else
        {
            color = sf::Color::Green; // 1 HP
        }
    }

} // namespace RebornGame
