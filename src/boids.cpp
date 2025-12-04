#include "boids.h"

Boids::Boids(const size_t population)
    : population{ population }
    , x(population), y(population)
    , vx(population), vy(population)
    , vertices(3 * population){}