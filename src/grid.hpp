#ifndef BOIDS_GRID_H
#define BOIDS_GRID_H

#include <vector>
#include <cmath>
#include <omp.h>

class NoLock {
protected:
    NoLock(size_t width, size_t height) {};

    void acquireLock(size_t square) {};
    void releaseLock(size_t square) {};
};

class Lock {
protected:
    Lock(size_t width, size_t height);
    ~Lock();

    void acquireLock(size_t square);
    void releaseLock(size_t square);

private:
    std::vector<omp_lock_t> locks;
};

template<typename LockPolicy = NoLock>
class Grid : private LockPolicy {
public:
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





inline Lock::Lock(const size_t width, const size_t height)
    : locks{width * height} {
    for (auto& lock : locks) {
        omp_init_lock(&lock);
    }
}

inline Lock::~Lock() {
    for (auto& lock : locks) {
        omp_destroy_lock(&lock);
    }
}

inline void Lock::acquireLock(const size_t square) {
    omp_set_lock(&locks[square]);
}

inline void Lock::releaseLock(const size_t square) {
    omp_unset_lock(&locks[square]);
}



template<typename LockPolicy>
Grid<LockPolicy>::Grid(const size_t width, const size_t height, const size_t squareSize)
    : LockPolicy{ width, height }
    , squareSize{ squareSize }
    , squaresPerRow{ (width % squareSize == 0) ? width / squareSize : width / squareSize + 1 }
    , squaresPerColumn{ (height % squareSize == 0) ? height / squareSize : height / squareSize + 1 }
    , squaresNumber{ squaresPerRow * squaresPerColumn }
    , grid(squaresNumber) {}

template<typename LockPolicy>
void Grid<LockPolicy>::add(const size_t index, const float x, const float y) {
    add(index, coords2square(x, y));
}

template<typename LockPolicy>
void Grid<LockPolicy>::add(const size_t index, const size_t square) {
    LockPolicy::acquireLock(square);
    grid[square].push_back(index);
    LockPolicy::releaseLock(square);
}

template<typename LockPolicy>
void Grid<LockPolicy>::remove(const size_t index, const float x, const float y) {
    remove(index, coords2square(x, y));
}

template<typename LockPolicy>
void Grid<LockPolicy>::remove(const size_t index, const size_t square) {
    LockPolicy::acquireLock(square);
    std::erase(grid[square], index);
    LockPolicy::releaseLock(square);
}

template<typename LockPolicy>
std::vector<size_t> Grid<LockPolicy>::getNeighbors(const float x, const float y) const {
    const auto occupiedSquare{ coords2square(x, y) };
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

template<typename LockPolicy>
size_t Grid<LockPolicy>::coords2square(const float x, const float y) const {
    return static_cast<size_t>(x) / squareSize + (static_cast<size_t>(y) / squareSize) * squaresPerRow;
}

template<typename LockPolicy>
bool Grid<LockPolicy>::isIndexValid(const size_t index) const {
    return index > -1 && index < squaresNumber;
}

#endif //BOIDS_GRID_H