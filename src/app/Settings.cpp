#include "Settings.hpp"

#include <fstream>
#include <sstream>

static Difficulty parseDifficulty(const std::string &s)
{
    if (s == "Easy")
        return Difficulty::Easy;
    if (s == "Hard")
        return Difficulty::Hard;
    return Difficulty::Normal;
}

static const char *difficultyToString(Difficulty d)
{
    switch (d)
    {
    case Difficulty::Easy:
        return "Easy";
    case Difficulty::Hard:
        return "Hard";
    case Difficulty::Normal:
    default:
        return "Normal";
    }
}

Settings Settings::loadFromFile(const std::string &path)
{
    Settings s;

    std::ifstream in(path);
    if (!in.is_open())
    {
        return s;
    }

    std::string line;
    while (std::getline(in, line))
    {
        if (line.empty())
            continue;
        if (line[0] == '#')
            continue;

        auto eq = line.find('=');
        if (eq == std::string::npos)
            continue;

        std::string key = line.substr(0, eq);
        std::string value = line.substr(eq + 1);

        if (key == "masterVolume")
        {
            try
            {
                s.masterVolume = std::stof(value);
                if (s.masterVolume < 0.0f)
                    s.masterVolume = 0.0f;
                if (s.masterVolume > 1.0f)
                    s.masterVolume = 1.0f;
            }
            catch (...)
            {
            }
        }
        else if (key == "difficulty")
        {
            s.difficulty = parseDifficulty(value);
        }
    }

    return s;
}

void Settings::saveToFile(const std::string &path) const
{
    std::ofstream out(path, std::ios::trunc);
    if (!out.is_open())
        return;

    out << "# CasseBriques settings\n";
    out << "masterVolume=" << masterVolume << "\n";
    out << "difficulty=" << difficultyToString(difficulty) << "\n";
}


