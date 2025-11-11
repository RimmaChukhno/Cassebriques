#include <iostream>
#include <SFML/Graphics.hpp>
#include "src/game/Game.h"
#include "src/game_reborn/GameReborn.h"

/**
 * @brief Affiche le menu de sélection
 * @return 1 pour Casse-Briques classique, 2 pour Reborn, 0 pour quitter
 */
int showMenu()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "     CASSE-BRIQUES - Menu Principal" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "1. Casse-Briques Classique" << std::endl;
    std::cout << "2. Casse-Briques Reborn" << std::endl;
    std::cout << "0. Quitter" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Votre choix : ";

    int choice;
    std::cin >> choice;
    return choice;
}

/**
 * @brief Point d'entrée principal
 */
int main()
{
    std::cout << "Bienvenue dans Casse-Bugues !" << std::endl;
    std::cout << "Le jeu où tu ne detruis pas seulement des briques," << std::endl;
    std::cout << "mais aussi les bugs qui hantent ton code." << std::endl;

    int choice;

    do
    {
        choice = showMenu();

        switch (choice)
        {
        case 1:
        {
            std::cout << "\nLancement du Casse-Briques Classique..." << std::endl;
            Game game;
            game.run();
            break;
        }
        case 2:
        {
            std::cout << "\nLancement du Casse-Briques Reborn..." << std::endl;
            GameReborn gameReborn;
            gameReborn.run();
            break;
        }
        case 0:
        {
            std::cout << "\nAu revoir !" << std::endl;
            break;
        }
        default:
        {
            std::cout << "\nChoix invalide. Veuillez reessayer." << std::endl;
            break;
        }
        }
    } while (choice != 0);

    return 0;
}
