#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <functional>

/**
 * @brief Classe pour créer un bouton cliquable
 */
class Button
{
private:
    sf::RectangleShape shape;
    sf::Text text;
    sf::Font font;
    sf::Color normalColor;
    sf::Color hoverColor;
    sf::Color pressedColor;
    bool isHovered;
    bool isPressed;

public:
    Button(const std::string &label, const sf::Font &font, float x, float y, float width, float height);
    
    void update(const sf::Vector2i &mousePosition, bool mousePressed);
    void draw(sf::RenderWindow &window) const;
    bool isClicked() const;
    bool getIsHovered() const { return isHovered; }
    void reset();
};

/**
 * @brief Classe principale du menu graphique
 */
class Menu
{
private:
    sf::RenderWindow window;
    sf::Font font;
    sf::Text titleText;
    std::vector<Button> buttons;
    int selectedOption;
    bool optionSelected;

public:
    Menu();
    ~Menu() = default;
    
    /**
     * @brief Affiche le menu et retourne le choix de l'utilisateur
     * @return 1 pour Classique, 2 pour Reborn, 0 pour quitter
     */
    int show();
    
private:
    void init();
    void handleEvents();
    void update();
    void draw();
    bool loadFont();
};

