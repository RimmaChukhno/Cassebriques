#include "InputManager.hpp"

InputManager::InputManager()
{
    // Initialiser tous les états à false
    for (int i = 0; i < sf::Keyboard::KeyCount; i++)
    {
        keyStates[static_cast<sf::Keyboard::Key>(i)] = false;
    }

    for (int i = 0; i < sf::Mouse::ButtonCount; i++)
    {
        mouseButtonStates[static_cast<sf::Mouse::Button>(i)] = false;
    }
}

void InputManager::processEvent(const sf::Event &event)
{
    if (event.type == sf::Event::KeyPressed)
    {
        keyStates[event.key.code] = true;

        // Appeler tous les callbacks pour cette touche
        if (keyPressedCallbacks.find(event.key.code) != keyPressedCallbacks.end())
        {
            for (auto &callback : keyPressedCallbacks[event.key.code])
            {
                callback();
            }
        }
    }
    else if (event.type == sf::Event::KeyReleased)
    {
        keyStates[event.key.code] = false;

        // Appeler tous les callbacks pour cette touche
        if (keyReleasedCallbacks.find(event.key.code) != keyReleasedCallbacks.end())
        {
            for (auto &callback : keyReleasedCallbacks[event.key.code])
            {
                callback();
            }
        }
    }
    else if (event.type == sf::Event::MouseButtonPressed)
    {
        mouseButtonStates[event.mouseButton.button] = true;

        // Appeler tous les callbacks pour ce bouton
        if (mousePressedCallbacks.find(event.mouseButton.button) != mousePressedCallbacks.end())
        {
            for (auto &callback : mousePressedCallbacks[event.mouseButton.button])
            {
                callback();
            }
        }
    }
    else if (event.type == sf::Event::MouseButtonReleased)
    {
        mouseButtonStates[event.mouseButton.button] = false;

        // Appeler tous les callbacks pour ce bouton
        if (mouseReleasedCallbacks.find(event.mouseButton.button) != mouseReleasedCallbacks.end())
        {
            for (auto &callback : mouseReleasedCallbacks[event.mouseButton.button])
            {
                callback();
            }
        }
    }
}

void InputManager::update(const sf::RenderWindow &window)
{
    // Mettre à jour la position de la souris
    mousePosition = sf::Mouse::getPosition(window);

    // Convertir en coordonnées du monde (si nécessaire)
    sf::Vector2f worldPos = window.mapPixelToCoords(mousePosition);
    mousePositionWorld = sf::Vector2i(static_cast<int>(worldPos.x), static_cast<int>(worldPos.y));
}

void InputManager::registerKeyPressedCallback(sf::Keyboard::Key key, std::function<void()> callback)
{
    keyPressedCallbacks[key].push_back(callback);
}

void InputManager::registerKeyReleasedCallback(sf::Keyboard::Key key, std::function<void()> callback)
{
    keyReleasedCallbacks[key].push_back(callback);
}

void InputManager::registerMousePressedCallback(sf::Mouse::Button button, std::function<void()> callback)
{
    mousePressedCallbacks[button].push_back(callback);
}

void InputManager::registerMouseReleasedCallback(sf::Mouse::Button button, std::function<void()> callback)
{
    mouseReleasedCallbacks[button].push_back(callback);
}

bool InputManager::isKeyPressed(sf::Keyboard::Key key) const
{
    auto it = keyStates.find(key);
    if (it != keyStates.end())
    {
        return it->second;
    }
    return false;
}

bool InputManager::isMouseButtonPressed(sf::Mouse::Button button) const
{
    auto it = mouseButtonStates.find(button);
    if (it != mouseButtonStates.end())
    {
        return it->second;
    }
    return false;
}

sf::Vector2i InputManager::getMousePosition() const
{
    return mousePosition;
}

sf::Vector2i InputManager::getMousePositionWorld() const
{
    return mousePositionWorld;
}
