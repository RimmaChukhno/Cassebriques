#pragma once

#include <cstdint>
#include <string>

enum class Difficulty : std::uint8_t
{
    Easy = 0,
    Normal = 1,
    Hard = 2,
};

struct Settings
{
    float masterVolume = 0.7f; // 0..1
    Difficulty difficulty = Difficulty::Normal;

    static Settings loadFromFile(const std::string &path);
    void saveToFile(const std::string &path) const;
};


