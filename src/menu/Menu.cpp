#include "Menu.hpp"
#include <iostream>

Button::Button(const std::string &label, const sf::Font &font, float x, float y, float width, float height)
    : font(font), normalColor(sf::Color(70, 70, 70)), 
      hoverColor(sf::Color(100, 100, 100)), pressedColor(sf::Color(50, 50, 50)),
      isHovered(false), isPressed(false)
{
    // Créer le rectangle du bouton
    shape.setSize(sf::Vector2f(width, height));
    shape.setPosition(x, y);
    shape.setFillColor(normalColor);
    shape.setOutlineColor(sf::Color(150, 150, 150));
    shape.setOutlineThickness(2.0f);
    
    // Créer le texte du bouton
    text.setFont(font);
    text.setString(label);
    text.setCharacterSize(32);
    text.setFillColor(sf::Color::White);
    
    // Centrer le texte dans le bouton
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setOrigin(textBounds.left + textBounds.width / 2.0f,
                   textBounds.top + textBounds.height / 2.0f);
    text.setPosition(x + width / 2.0f, y + height / 2.0f);
}

void Button::update(const sf::Vector2i &mousePosition, bool mousePressed)
{
    sf::FloatRect bounds = shape.getGlobalBounds();
    
    isHovered = bounds.contains(static_cast<float>(mousePosition.x), 
                                 static_cast<float>(mousePosition.y));
    
    if (isHovered && mousePressed)
    {
        isPressed = true;
        shape.setFillColor(pressedColor);
    }
    else if (isHovered)
    {
        shape.setFillColor(hoverColor);
    }
    else
    {
        shape.setFillColor(normalColor);
    }
}

void Button::draw(sf::RenderWindow &window) const
{
    window.draw(shape);
    window.draw(text);
}

bool Button::isClicked() const
{
    return isHovered && isPressed;
}

void Button::reset()
{
    isPressed = false;
    isHovered = false;
    shape.setFillColor(normalColor);
}

Menu::Menu()
    : window(sf::VideoMode(800, 600), "Casse-Briques - Menu Principal"),
      selectedOption(0), optionSelected(false)
{
    window.setFramerateLimit(60);
    init();
}

void Menu::init()
{
    if (!loadFont())
    {
        std::cerr << "Attention: Impossible de charger la police pour le menu. Le texte ne s'affichera pas correctement." << std::endl;
        std::cerr << "Le menu fonctionnera toujours avec les touches du clavier (1, 2, 0)" << std::endl;
    }
    
    // Titre
    titleText.setFont(font);
    titleText.setString("CASSE-BRIQUES");
    titleText.setCharacterSize(72);
    titleText.setFillColor(sf::Color(255, 200, 0)); // Couleur dorée
    titleText.setStyle(sf::Text::Bold);
    
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin(titleBounds.left + titleBounds.width / 2.0f,
                        titleBounds.top + titleBounds.height / 2.0f);
    titleText.setPosition(400.0f, 80.0f);
    
    // Créer les boutons
    float buttonWidth = 400.0f;
    float buttonHeight = 60.0f;
    float startY = 250.0f;
    float spacing = 80.0f;
    
    buttons.push_back(Button("1. Casse-Briques Classique", font, 
                             400.0f - buttonWidth / 2.0f, startY, 
                             buttonWidth, buttonHeight));
    
    buttons.push_back(Button("2. Casse-Briques Reborn", font,
                             400.0f - buttonWidth / 2.0f, startY + spacing,
                             buttonWidth, buttonHeight));
    
    buttons.push_back(Button("0. Quitter", font,
                             400.0f - buttonWidth / 2.0f, startY + spacing * 2,
                             buttonWidth, buttonHeight));
}

bool Menu::loadFont()
{
    bool fontLoaded = false;
    
    // Essayer de charger depuis assets/fonts
    if (!fontLoaded)
    {
        fontLoaded = font.loadFromFile("assets/fonts/arial.ttf");
    }
    
    #ifdef _WIN32
    // Essayer les polices système Windows
    if (!fontLoaded)
    {
        fontLoaded = font.loadFromFile("C:/Windows/Fonts/arial.ttf");
    }
    if (!fontLoaded)
    {
        fontLoaded = font.loadFromFile("C:/Windows/Fonts/calibri.ttf");
    }
    if (!fontLoaded)
    {
        fontLoaded = font.loadFromFile("C:/Windows/Fonts/times.ttf");
    }
    #endif
    
    return fontLoaded;
}

int Menu::show()
{
    while (window.isOpen() && !optionSelected)
    {
        handleEvents();
        update();
        draw();
    }
    
    return selectedOption;
}

void Menu::handleEvents()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            selectedOption = 0;
            optionSelected = true;
            window.close();
        }
        
        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Escape)
            {
                selectedOption = 0;
                optionSelected = true;
                window.close();
            }
            else if (event.key.code == sf::Keyboard::Num1 || event.key.code == sf::Keyboard::Numpad1)
            {
                selectedOption = 1;
                optionSelected = true;
                window.close();
            }
            else if (event.key.code == sf::Keyboard::Num2 || event.key.code == sf::Keyboard::Numpad2)
            {
                selectedOption = 2;
                optionSelected = true;
                window.close();
            }
        }
    }
}

void Menu::update()
{
    static bool wasMousePressed = false;
    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
    bool mousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);
    
    for (size_t i = 0; i < buttons.size(); ++i)
    {
        buttons[i].update(mousePosition, mousePressed);
        
        // Détecter un clic (pressé puis relâché)
        if (wasMousePressed && !mousePressed && buttons[i].getIsHovered())
        {
            // Le bouton a été cliqué
            if (i == 0)
            {
                selectedOption = 1;
            }
            else if (i == 1)
            {
                selectedOption = 2;
            }
            else if (i == 2)
            {
                selectedOption = 0;
            }
            optionSelected = true;
            window.close();
            break;
        }
    }
    
    wasMousePressed = mousePressed;
}

void Menu::draw()
{
    // Fond avec dégradé (simulé avec un rectangle)
    window.clear(sf::Color(20, 20, 40)); // Fond bleu foncé
    
    // Ligne décorative en haut
    sf::RectangleShape topLine(sf::Vector2f(800.0f, 3.0f));
    topLine.setPosition(0.0f, 0.0f);
    topLine.setFillColor(sf::Color(255, 200, 0));
    window.draw(topLine);
    
    // Dessiner le titre
    window.draw(titleText);
    
    // Dessiner le sous-titre
    sf::Text subtitleText("Menu Principal", font, 36);
    subtitleText.setFillColor(sf::Color(200, 200, 200));
    subtitleText.setStyle(sf::Text::Italic);
    
    sf::FloatRect subtitleBounds = subtitleText.getLocalBounds();
    subtitleText.setOrigin(subtitleBounds.left + subtitleBounds.width / 2.0f,
                          subtitleBounds.top + subtitleBounds.height / 2.0f);
    subtitleText.setPosition(400.0f, 150.0f);
    window.draw(subtitleText);
    
    // Ligne de séparation
    sf::RectangleShape separator(sf::Vector2f(600.0f, 2.0f));
    separator.setPosition(100.0f, 200.0f);
    separator.setFillColor(sf::Color(100, 100, 100));
    window.draw(separator);
    
    // Dessiner les boutons
    for (const auto &button : buttons)
    {
        button.draw(window);
    }
    
    // Instructions en bas
    sf::Text instructions("Utilisez la souris pour selectionner ou appuyez sur 1, 2 ou 0", font, 18);
    instructions.setFillColor(sf::Color(150, 150, 150));
    
    sf::FloatRect instBounds = instructions.getLocalBounds();
    instructions.setOrigin(instBounds.left + instBounds.width / 2.0f,
                           instBounds.top + instBounds.height / 2.0f);
    instructions.setPosition(400.0f, 550.0f);
    window.draw(instructions);
    
    window.display();
}

