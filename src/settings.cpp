#include "settings.h"

#include <algorithm>
#include <fstream>
#include <iostream>

namespace {
    Settings::Settings getSettings(const std::string& path) {
        std::ifstream in(path);
        if (!in.is_open()) {
            std::cerr << "Could not open file " << path << std::endl;
            exit(-1);
        }

        Settings::Settings settings;
        while(in.good()) {
            std::string word;
            in >> word;
            if (word.starts_with("#")) {
                in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            std::ranges::transform(word, word.begin(), [](const unsigned char c) { return std::toupper(c); });
            if (word == "POPULATION") {
                in >> settings.population;
            }
            else if (word == "MAXRUN") {
                in >> settings.maxRunNumber;
            }
            else if (word == "SCREEN") {
                in >> settings.screenWidth >> settings.screenHeight;
                in >> settings.clearRed >> settings.clearGreen >> settings.clearBlue;
            }
            else if (word == "NOVSYNC") {
                settings.disableVSync = true;
            }
            else if (word == "BOIDS") {
                in >> settings.boidsLength >> settings.boidsWidth;
                in >> settings.boidsColor.r >> settings.boidsColor.g >> settings.boidsColor.b;
            }
            else if (word == "VELOCITY") {
                in >> settings.minVelocity >> settings.maxVelocity;
            }
            else if (word == "RANGES") {
                in >> settings.visibleRange >> settings.dangerRange;
                settings.visibleRangeSquared = settings.visibleRange * settings.visibleRange;
                settings.dangerRangeSquared = settings.dangerRange * settings.dangerRange;
            }
            else if (word == "ALIGNMENT") {
                in >> settings.alignmentFactor;
            }
            else if (word == "COHESION") {
                in >> settings.cohesionFactor;
            }
            else if (word == "DANGER") {
                in >> settings.dangerFactor;
            }
            else if (word == "MARGIN") {
                in >> settings.margin;
            }
            else if (word == "TURN") {
                in >> settings.turnSpeed;
            }
        }
        return settings;
    }

    void checkSettings(const Settings::Settings& settings) {
        if (settings.population < 1) {
            std::cerr << "Population should be at least 1, but was " << settings.population << std::endl;
            exit(-1);
        }
        if (settings.maxVelocity < settings.minVelocity) {
            std::cerr << "Max velocity should not be less than min velocity" << std::endl;
            std::cerr << "Max velocity was " << settings.maxVelocity << ", ";
            std::cerr << "min velocity was " << settings.minVelocity << std::endl;
            exit(-1);
        }
        if (settings.boidsLength <= 0 || settings.boidsWidth <= 0) {
            std::cerr << "Boids length and width can not be lower or equal to 0." << std::endl;
            std::cerr << "Boids length was " << settings.boidsLength << ", ";
            std::cerr << "boids width was " << settings.boidsWidth << std::endl;
            exit(-1);
        }
        if (settings.visibleRange < settings.dangerRange) {
            std::cerr << "Visible range should not be less than danger range." << std::endl;
            std::cerr << "Visible range was " << settings.visibleRange << ", ";
            std::cerr << "danger range was " << settings.dangerRange << std::endl;
            exit(-1);
        }
        if (settings.boidsColor.r < 0 || settings.boidsColor.g < 0 || settings.boidsColor.b < 0 ||
            settings.boidsColor.r > 1 || settings.boidsColor.g > 1 || settings.boidsColor.b > 1) {
            std::cerr << "Boids color channels should be between 0 and 1." << std::endl;
            std::cerr << "Red channel was " << settings.boidsColor.r << ", ";
            std::cerr << "green channel was " << settings.boidsColor.g << ", ";
            std::cerr << "blue channel was " << settings.boidsColor.b << std::endl;
            exit(-1);
        }
    }
}

Settings::Settings Settings::loadSettings(const std::string& path) {
    auto settings{ getSettings(path) };
    settings.boidsColor.a = SDL_ALPHA_OPAQUE_FLOAT;
    checkSettings(settings);
    return settings;
}