#include "AABB.hpp"

bool AABB::intersects(const AABB &other) const
{
    return !(right < other.left || left > other.right ||
             bottom < other.top || top > other.bottom);
}

float AABB::getWidth() const
{
    return right - left;
}

float AABB::getHeight() const
{
    return bottom - top;
}

