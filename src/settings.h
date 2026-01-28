#ifndef BOIDS_SETTINGS_H
#define BOIDS_SETTINGS_H

#include <string>
#include "SDL3/SDL_pixels.h"

// Struct holding all the application settings
//
// For more information about the settings refer to the README.
struct Settings {
    size_t population{};
    size_t screenWidth {};
    size_t screenHeight{};
    size_t maxRunNumber{};
    size_t threadsNumber{};
    size_t neighborLoopChunkSize{};
    bool disableVSync{};
    float boidsLength{};
    float boidsWidth{};
    float minVelocity{}, maxVelocity{};
    SDL_FColor boidsColor{};
    std::uint8_t clearRed{}, clearGreen{}, clearBlue{};
    float visibleRange{};
    float visibleRangeSquared{ visibleRange * visibleRange };
    float dangerRange{};
    float dangerRangeSquared{ dangerRange * dangerRange };
    float alignmentFactor{};
    float cohesionFactor{};
    float dangerFactor{};
    size_t margin{};
    float turnSpeed{};
};

// Load the settings from the file found at "path" and verify them.
//
// If there is no file at "path" print an error string and exit the program.
// If a setting has an invalid value print an error string and exit the program.
Settings loadSettings(const std::string& path);

#endif //BOIDS_SETTINGS_H