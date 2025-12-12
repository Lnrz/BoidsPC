#ifndef BOIDS_SETTINGS_H
#define BOIDS_SETTINGS_H

#include <cstdint>
#include <string>

#include "SDL3/SDL_pixels.h"

namespace Settings {
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

    Settings loadSettings(const std::string& path);
}

#endif //BOIDS_SETTINGS_H