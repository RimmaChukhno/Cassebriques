#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <functional>
#include <map>
#include <vector>

/**
 * @brief Gestionnaire d'entrées (clavier et souris)
 *
 * Permet d'enregistrer des callbacks pour réagir aux événements
 */
class InputManager
{
private:
    // Callbacks pour les touches du clavier
    std::map<sf::Keyboard::Key, std::vector<std::function<void()>>> keyPressedCallbacks;
    std::map<sf::Keyboard::Key, std::vector<std::function<void()>>> keyReleasedCallbacks;

    // Callbacks pour les boutons de la souris
    std::map<sf::Mouse::Button, std::vector<std::function<void()>>> mousePressedCallbacks;
    std::map<sf::Mouse::Button, std::vector<std::function<void()>>> mouseReleasedCallbacks;

    // État actuel des touches et boutons
    std::map<sf::Keyboard::Key, bool> keyStates;
    std::map<sf::Mouse::Button, bool> mouseButtonStates;

    // Position de la souris
    sf::Vector2i mousePosition;
    sf::Vector2i mousePositionWorld; // Position dans le monde (si vue transformée)

public:
    InputManager();
    ~InputManager() = default;

    /**
     * @brief Traite un événement SFML
     * @param event L'événement à traiter
     */
    void processEvent(const sf::Event &event);

    /**
     * @brief Met à jour l'état (position de la souris, etc.)
     * @param window Fenêtre SFML pour obtenir la position de la souris
     */
    void update(const sf::RenderWindow &window);

    /**
     * @brief Enregistre un callback pour une touche pressée
     * @param key La touche
     * @param callback La fonction à appeler
     */
    void registerKeyPressedCallback(sf::Keyboard::Key key, std::function<void()> callback);

    /**
     * @brief Enregistre un callback pour une touche relâchée
     * @param key La touche
     * @param callback La fonction à appeler
     */
    void registerKeyReleasedCallback(sf::Keyboard::Key key, std::function<void()> callback);

    /**
     * @brief Enregistre un callback pour un bouton de souris pressé
     * @param button Le bouton
     * @param callback La fonction à appeler
     */
    void registerMousePressedCallback(sf::Mouse::Button button, std::function<void()> callback);

    /**
     * @brief Enregistre un callback pour un bouton de souris relâché
     * @param button Le bouton
     * @param callback La fonction à appeler
     */
    void registerMouseReleasedCallback(sf::Mouse::Button button, std::function<void()> callback);

    /**
     * @brief Vérifie si une touche est actuellement pressée
     * @param key La touche à vérifier
     * @return true si pressée, false sinon
     */
    bool isKeyPressed(sf::Keyboard::Key key) const;

    /**
     * @brief Vérifie si un bouton de souris est actuellement pressé
     * @param button Le bouton à vérifier
     * @return true si pressé, false sinon
     */
    bool isMouseButtonPressed(sf::Mouse::Button button) const;

    /**
     * @brief Retourne la position de la souris (en pixels de la fenêtre)
     */
    sf::Vector2i getMousePosition() const;

    /**
     * @brief Retourne la position de la souris dans le monde (si vue transformée)
     */
    sf::Vector2i getMousePositionWorld() const;
};

