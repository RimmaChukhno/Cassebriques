#pragma once

#include <SFML/Graphics.hpp>
#include <string>

class Button
{
public:
    Button() = default;
    Button(const sf::Font *font,
           const std::string &label,
           const sf::Vector2f &pos,
           const sf::Vector2f &size);

    void setLabel(const std::string &label);
    void setSelected(bool selected);

    // Feed mouse + click edge (pressed/released) info
    void update(const sf::Vector2f &mousePos, bool mouseDown);
    bool consumeClick(); // true once per click-release

    void render(sf::RenderTarget &target) const;

    bool isHovered() const { return hovered; }

private:
    const sf::Font *font = nullptr;
    sf::RectangleShape box;
    sf::Text text;

    bool hovered = false;
    bool pressed = false;
    bool selected = false;
    bool clickReady = false;

    void updateColors();
    void centerText();
};


