#include "Assets.hpp"

#include <iostream>

static bool looksLikeRoot(const std::filesystem::path &p)
{
    return std::filesystem::exists(p / "assets") && std::filesystem::exists(p / "levels");
}

static std::filesystem::path findRootDir()
{
    std::filesystem::path p = std::filesystem::current_path();

    // Walk upward until we find a folder containing assets/ and levels/
    for (int i = 0; i < 8; i++)
    {
        if (looksLikeRoot(p))
            return p;
        if (!p.has_parent_path())
            break;
        p = p.parent_path();
    }

    // Fallback: use cwd even if it doesn't contain assets
    return std::filesystem::current_path();
}

static bool tryLoadFont(sf::Font &font, const std::filesystem::path &p)
{
    if (!std::filesystem::exists(p))
        return false;
    return font.loadFromFile(p.string());
}

void Assets::init()
{
    rootDir = findRootDir();

    // Try bundled font first (if user adds one later)
    uiFontLoaded = tryLoadFont(uiFont, assetsDir() / "fonts" / "arial.ttf");

#ifdef _WIN32
    // Windows system fonts fallback
    if (!uiFontLoaded)
        uiFontLoaded = tryLoadFont(uiFont, "C:/Windows/Fonts/segoeui.ttf");
    if (!uiFontLoaded)
        uiFontLoaded = tryLoadFont(uiFont, "C:/Windows/Fonts/arial.ttf");
    if (!uiFontLoaded)
        uiFontLoaded = tryLoadFont(uiFont, "C:/Windows/Fonts/calibri.ttf");
#endif

    // Diagnostics (dev-friendly)
    std::cout << "[Assets] cwd=" << std::filesystem::current_path().string() << "\n";
    std::cout << "[Assets] rootDir=" << rootDir.string() << "\n";
    std::cout << "[Assets] uiFontLoaded=" << (uiFontLoaded ? "true" : "false") << "\n";
}


