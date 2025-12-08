#ifndef BOIDS_BOIDS_H
#define BOIDS_BOIDS_H

#include <vector>
#include <SDL3/SDL.h>

struct Boids {
    explicit Boids(size_t population);

    const size_t population;
    std::vector<float> x;
    std::vector<float> y;
    std::vector<float> vx;
    std::vector<float> vy;
    std::vector<SDL_Vertex> vertices;
};

#endif //BOIDS_BOIDS_H