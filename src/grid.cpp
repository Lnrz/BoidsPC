#include "grid.h"

#include <cmath>

Grid::Grid(const size_t width, const size_t height, const size_t squareSize)
    : squareSize{ squareSize }
    , squaresPerRow{ (width % squareSize == 0) ? width / squareSize : width / squareSize + 1 }
    , squaresPerColumn{ (height % squareSize == 0) ? height / squareSize : height / squareSize + 1 }
    , squaresNumber{ squaresPerRow * squaresPerColumn }
    , grid(squaresNumber) {}

void Grid::add(const size_t index, const float x, const float y) {
    add(index, coords2square(x, y));
}

void Grid::add(const size_t index, const size_t square) {
    grid[square].push_back(index);
}

void Grid::remove(const size_t index, const float x, const float y) {
    remove(index, coords2square(x, y));
}

void Grid::remove(const size_t index, const size_t square) {
    std::erase(grid[square], index);
}

std::vector<size_t> Grid::getNeighbors(const float x, const float y) const {
    const auto occupiedSquare = coords2square(x, y);
    float trash{};
    const bool isTowardsRight{ modf(x / static_cast<float>(squareSize), &trash) >= 0.5};
    const bool isTowardsTop{ modf(y / static_cast<float>(squareSize), &trash) >= 0.5};
    std::vector<size_t> neighbors;

    neighbors.insert(neighbors.cend(), grid[occupiedSquare].cbegin(), grid[occupiedSquare].cend());
    if (const size_t index{ occupiedSquare + 1 } ; isTowardsRight && isIndexValid(index)) {
        neighbors.insert(neighbors.cend(), grid[index].cbegin(), grid[index].cend());
    }
    if (const size_t index{ occupiedSquare - 1 } ; !isTowardsRight && isIndexValid(index)) {
        neighbors.insert(neighbors.cend(), grid[index].cbegin(), grid[index].cend());
    }
    if (const size_t index{ occupiedSquare - squaresPerRow } ; isTowardsTop && isIndexValid(index)) {
        neighbors.insert(neighbors.cend(), grid[index].cbegin(), grid[index].cend());
    }
    if (const size_t index{ occupiedSquare + squaresPerRow } ; !isTowardsTop && isIndexValid(index)) {
        neighbors.insert(neighbors.cend(), grid[index].cbegin(), grid[index].cend());
    }
    if (const size_t index{ occupiedSquare + 1 - squaresPerRow } ; isTowardsRight && isTowardsTop && isIndexValid(index)) {
        neighbors.insert(neighbors.cend(), grid[index].cbegin(), grid[index].cend());
    }
    if (const size_t index{ occupiedSquare + squaresPerRow - 1 } ; !isTowardsRight && !isTowardsTop && isIndexValid(index)) {
        neighbors.insert(neighbors.cend(), grid[index].cbegin(), grid[index].cend());
    }
    if (const size_t index{ occupiedSquare - squaresPerRow - 1 } ; !isTowardsRight && isTowardsTop && isIndexValid(index)) {
        neighbors.insert(neighbors.cend(), grid[index].cbegin(), grid[index].cend());
    }
    if (const size_t index{ occupiedSquare + squaresPerRow + 1} ; isTowardsRight && !isTowardsTop && isIndexValid(index)) {
        neighbors.insert(neighbors.cend(), grid[index].cbegin(), grid[index].cend());
    }

    return neighbors;
}

size_t Grid::coords2square(const float x, const float y) const {
    return static_cast<size_t>(x) / squareSize + (static_cast<size_t>(y) / squareSize) * squaresPerRow;
}

bool Grid::isIndexValid(const size_t index) const {
    return index > -1 && index < squaresNumber;
}
