#ifndef BOIDS_GRID_H
#define BOIDS_GRID_H

#include <vector>

struct Grid {
    Grid(size_t width, size_t height, size_t squareSize);

    void add(size_t index, float x, float y);
    void add(size_t index, size_t square);

    void remove(size_t index, float x, float y);
    void remove(size_t index, size_t square);

    [[nodiscard]]
    std::vector<size_t> getNeighbors(float x, float y) const;

    [[nodiscard]]
    size_t coords2square(float x, float y) const;

private:
    [[nodiscard]]
    bool isIndexValid(size_t index) const;

    const size_t squareSize;
    const size_t squaresPerRow;
    const size_t squaresPerColumn;
    const size_t squaresNumber;
    std::vector<std::vector<size_t>> grid;
};

#endif //BOIDS_GRID_H