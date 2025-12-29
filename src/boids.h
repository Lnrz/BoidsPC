#ifndef BOIDS_BOIDS_H
#define BOIDS_BOIDS_H

#include <vector>
#include <SDL3/SDL.h>

// Struct holding all the data related to boids.
struct Boids {
    explicit Boids(size_t population);

    const size_t population;
    std::vector<float> x;
    std::vector<float> y;
    std::vector<float> vx;
    std::vector<float> vy;
    std::vector<float> cohesionx;
    std::vector<float> cohesiony;
    std::vector<float> alignmentx;
    std::vector<float> alignmenty;
    std::vector<float> dangerx;
    std::vector<float> dangery;
    std::vector<float> turnx;
    std::vector<float> turny;
    std::vector<SDL_Vertex> vertices;
};

#endif //BOIDS_BOIDS_H