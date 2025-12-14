#include "Button.hpp"

Button::Button(const sf::Font *font,
               const std::string &label,
               const sf::Vector2f &pos,
               const sf::Vector2f &size)
    : font(font)
{
    box.setPosition(pos);
    box.setSize(size);
    box.setOutlineThickness(2.0f);
    box.setOutlineColor(sf::Color(110, 110, 130));

    if (font)
    {
        text.setFont(*font);
        text.setCharacterSize(26);
        text.setString(label);
        text.setFillColor(sf::Color::White);
        centerText();
    }

    updateColors();
}

void Button::setLabel(const std::string &label)
{
    text.setString(label);
    centerText();
}

void Button::setSelected(bool s)
{
    selected = s;
    updateColors();
}

void Button::centerText()
{
    sf::FloatRect b = text.getLocalBounds();
    text.setOrigin(b.left + b.width / 2.0f, b.top + b.height / 2.0f);
    const sf::Vector2f pos = box.getPosition();
    const sf::Vector2f size = box.getSize();
    text.setPosition(pos.x + size.x / 2.0f, pos.y + size.y / 2.0f);
}

void Button::updateColors()
{
    // Base theme
    sf::Color fill(45, 45, 70);
    sf::Color outline(110, 110, 130);

    if (selected)
        outline = sf::Color(255, 200, 0);
    if (hovered)
        fill = sf::Color(60, 60, 95);
    if (pressed)
        fill = sf::Color(35, 35, 55);

    box.setFillColor(fill);
    box.setOutlineColor(outline);
}

void Button::update(const sf::Vector2f &mousePos, bool mouseDown)
{
    const bool contains = box.getGlobalBounds().contains(mousePos);

    // Track click on release
    if (!mouseDown && pressed && contains)
    {
        clickReady = true;
    }

    hovered = contains;
    pressed = hovered && mouseDown;
    updateColors();
}

bool Button::consumeClick()
{
    if (!clickReady)
        return false;
    clickReady = false;
    return true;
}

void Button::render(sf::RenderTarget &target) const
{
    target.draw(box);
    if (font)
        target.draw(text);
}


