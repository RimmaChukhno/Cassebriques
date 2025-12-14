#pragma once

#include <SFML/Graphics.hpp>

#include <filesystem>

struct Assets
{
    sf::Font uiFont;
    bool uiFontLoaded = false;

    std::filesystem::path rootDir; // directory that contains assets/ and levels/

    // Finds a usable root dir (walks upward from cwd) and tries to load fonts.
    void init();

    std::filesystem::path assetsDir() const { return rootDir / "assets"; }
    std::filesystem::path levelsDir() const { return rootDir / "levels"; }
};


