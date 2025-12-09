#include "boids.h"

Boids::Boids(const size_t population)
    : population{ population }
    , x(population), y(population)
    , vx(population), vy(population)
    , cohesionx(population), cohesiony(population)
    , alignmentx(population), alignmenty(population)
    , dangerx(population), dangery(population)
    , turnx(population), turny(population)
    , vertices(3 * population) {}