#include "Brick.hpp"

namespace ClassicGame
{

    Brick::Brick(float x, float y, float width, float height, const sf::Color &col, int pts)
        : GameObject(x, y, width, height, col), points(pts), destroyed(false)
    {
    }

    bool Brick::isDestroyed() const
    {
        return destroyed;
    }

    void Brick::destroy()
    {
        destroyed = true;
    }

    int Brick::getPoints() const
    {
        return points;
    }

} // namespace ClassicGame
