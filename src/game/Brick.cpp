#include "Brick.h"

namespace ClassicGame
{

    Brick::Brick(float x, float y, float width, float height, const sf::Color &col, int pts)
        : GameObject(x, y, width, height, col), points(pts), destroyed(false)
    {
    }

} // namespace ClassicGame
