#include <algorithm>
#include <numbers>
#include <random>
#include <SDL3/SDL.h>
#include "boids.h"
#include "grid.hpp"
#include "settings.h"
#include "stats.h"

template<typename LockPolicy>
void randomizeBoids(Boids& boids, Grid<LockPolicy>& grid, const Settings::Settings& settings) {
    std::mt19937 generator{ std::random_device{}() };
    for (size_t i{ 0 }; i < boids.population; i++) {
        boids.x[i] = std::fmodf(static_cast<float>(generator()), static_cast<float>(settings.screenWidth) - .1f);
        boids.y[i] = std::fmodf(static_cast<float>(generator()), static_cast<float>(settings.screenHeight) - .1f);
        grid.add(i, boids.x[i], boids.y[i]);
        const float angle{ std::fmodf(static_cast<float>(generator()), 2 * std::numbers::pi) };
        boids.vx[i] = std::cos(angle) * settings.minVelocity;
        boids.vy[i] = std::sin(angle) * settings.minVelocity;
        boids.vertices[3 * i].color = settings.boidsColor;
        boids.vertices[3 * i + 1].color = settings.boidsColor;
        boids.vertices[3 * i + 2].color = settings.boidsColor;
    }
}

float calculateSquaredDistance(const float x1, const float y1, const float x2, const float y2) {
    return powf(x1 - x2, 2) + powf(y2 - y1, 2);
}

float calculateNorm(const float x, const float y) {
    return std::sqrtf(x * x + y * y);
}

template<typename LockPolicy>
void updateBoidsVelocities(Boids& boids, const Grid<LockPolicy>& grid, const Settings::Settings& settings) {
#pragma omp for schedule(static)
    for (int i{ 0 }; i < boids.population; i++) {
        size_t visibleBoidsNum{ 0 };
        float dangerX{ 0 };
        float dangerY{ 0 };
        float averageX { boids.x[i] };
        float averageY { boids.y[i] };
        float averageVX { boids.vx[i] };
        float averageVY { boids.vy[i] };

        // Compute alignment, danger and cohesion velocity modifiers
        for (const auto neighborIndex : grid.getNeighbors(boids.x[i], boids.y[i])) {
            const auto squaredDistance{
                calculateSquaredDistance(boids.x[i], boids.y[i],
                                        boids.x[neighborIndex], boids.y[neighborIndex])
            };
            if (squaredDistance > settings.visibleRangeSquared) continue;
            if (squaredDistance > settings.dangerRangeSquared) {
                averageX += boids.x[neighborIndex];
                averageY += boids.y[neighborIndex];
                averageVX += boids.vx[neighborIndex];
                averageVY += boids.vy[neighborIndex];
                visibleBoidsNum++;
            } else {
                dangerX += boids.x[i] - boids.x[neighborIndex];
                dangerY += boids.y[i] - boids.y[neighborIndex];
            }
        }
        if (visibleBoidsNum > 0) {
            averageX = (averageX - boids.x[i]) / static_cast<float>(visibleBoidsNum);
            averageY = (averageY - boids.y[i]) / static_cast<float>(visibleBoidsNum);
            averageVX = (averageVX - boids.vx[i]) / static_cast<float>(visibleBoidsNum);
            averageVY = (averageVY - boids.vy[i]) / static_cast<float>(visibleBoidsNum);
        }

        // Check if close to border
        float xTurnFactor { 0 }, yTurnFactor{ 0 };
        if (boids.x[i] < static_cast<float>(settings.margin)) xTurnFactor = 1;
        else if (boids.x[i] > static_cast<float>(settings.screenWidth - settings.margin)) xTurnFactor = -1;
        if (boids.y[i] < static_cast<float>(settings.margin)) yTurnFactor = 1;
        else if (boids.y[i] > static_cast<float>(settings.screenHeight - settings.margin)) yTurnFactor = -1;

        // Compute velocity
        boids.vx[i] += settings.dangerFactor * dangerX +
                       settings.cohesionFactor * (averageX - boids.x[i]) +
                       settings.alignmentFactor * (averageVX - boids.vx[i]) +
                       settings.turnSpeed * xTurnFactor;
        boids.vy[i] += settings.dangerFactor * dangerY +
                       settings.cohesionFactor * (averageY - boids.y[i]) +
                       settings.alignmentFactor * (averageVY - boids.vy[i]) +
                       settings.turnSpeed * yTurnFactor;

        // Bound velocity
        if (const auto velocityNorm{ calculateNorm(boids.vx[i], boids.vy[i]) }; velocityNorm < settings.minVelocity && velocityNorm > 0) {
            boids.vx[i] = settings.minVelocity * boids.vx[i] / velocityNorm;
            boids.vy[i] = settings.minVelocity * boids.vy[i] / velocityNorm;
        } else if (velocityNorm > settings.maxVelocity) {
            boids.vx[i] = settings.maxVelocity * boids.vx[i] / velocityNorm;
            boids.vy[i] = settings.maxVelocity * boids.vy[i] / velocityNorm;
        }
    }
}

template<typename LockPolicy>
void updateBoidsPositions(Boids& boids, Grid<LockPolicy>& grid, const Settings::Settings& settings, const uint64_t elapsedMs) {
    const float delta{ elapsedMs > 0 ? 1.0f / static_cast<float>(elapsedMs) : 1000.0f / 60.0f };
#pragma omp  for schedule(static)
    for (int i{ 0 }; i < boids.population; i++) {
        boids.x[i] += delta * boids.vx[i];
        boids.y[i] += delta * boids.vy[i];
    }
#pragma omp for schedule(static)
    for (int i{ 0 }; i < boids.population; i++) {
        const auto prevSquare{ grid.coords2square(boids.x[i] - delta * boids.vx[i], boids.y[i] - delta * boids.vy[i]) };
        boids.x[i] = std::clamp(boids.x[i], 0.0f, static_cast<float>(settings.screenWidth) - .1f);
        boids.y[i] = std::clamp(boids.y[i], 0.0f, static_cast<float>(settings.screenHeight) - .1f);
        const auto nextSquare{ grid.coords2square(boids.x[i], boids.y[i]) };
        if (prevSquare != nextSquare) {
            grid.remove(i, prevSquare);
            grid.add(i, nextSquare);
        }
    }
}

void updateBoidsVertices(Boids& boids, SDL_Renderer* renderer, const Settings::Settings& settings) {
    for (size_t i{ 0 }; i < boids.population; i++) {
        const float norm{ calculateNorm(boids.vx[i], boids.vy[i]) };
        if (norm == 0.0f) continue;
        const float normalizedVx{ boids.vx[i] / norm };
        const float normalizedVy{ boids.vy[i] / norm };
        const float cos120{ std::cosf(std::numbers::pi / 3. * 2.) };
        const float sin120{ std::sinf(std::numbers::pi / 3. * 2.) };
        SDL_RenderCoordinatesFromWindow(renderer,
            boids.x[i] + settings.boidsLength * normalizedVx,
            boids.y[i] + settings.boidsLength * normalizedVy,
            &boids.vertices[3 * i].position.x,
            &boids.vertices[3 * i].position.y);
        SDL_RenderCoordinatesFromWindow(renderer,
            boids.x[i] + settings.boidsWidth * (cos120 * normalizedVx - sin120 * normalizedVy),
            boids.y[i] + settings.boidsWidth * (sin120 * normalizedVx + cos120 * normalizedVy),
            &boids.vertices[3 * i + 1].position.x,
            &boids.vertices[3 * i + 1].position.y);
        SDL_RenderCoordinatesFromWindow(renderer,
            boids.x[i] + settings.boidsWidth * (cos120 * normalizedVx + sin120 * normalizedVy),
            boids.y[i] + settings.boidsWidth * (-sin120 * normalizedVx + cos120 * normalizedVy),
            &boids.vertices[3 * i + 2].position.x,
            &boids.vertices[3 * i + 2].position.y);
    }
}

int main(int argc, char* argv[]) {
    const auto settings{ Settings::loadSettings(argc > 1 ? argv[1] : "settings.txt") };
    Stats stats{ "log.txt" };
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_CreateWindowAndRenderer("Boids", static_cast<int>(settings.screenWidth), static_cast<int>(settings.screenHeight), 0, &window, &renderer);
    SDL_SetRenderVSync(renderer, 1);
    SDL_SetRenderDrawColor(renderer, settings.clearRed, settings.clearGreen, settings.clearBlue, SDL_ALPHA_OPAQUE);
    bool isQuitRequested{ false };

    Boids boids{ settings.population };
#ifdef _OPENMP
    Grid<Lock> grid{ settings.screenWidth, settings.screenHeight, static_cast<size_t>(ceilf(settings.visibleRange)) };
#else
    Grid grid{ settings.screenWidth, settings.screenHeight, static_cast<size_t>(ceilf(settings.visibleRange)) };
#endif
    randomizeBoids(boids, grid, settings);

    size_t runNumber{ 0 };
    auto lastFrameStartTick{ SDL_GetTicks() };
    decltype(lastFrameStartTick) currentFrameStartTick{};
#pragma omp parallel default(none) \
    shared(boids, grid, lastFrameStartTick, currentFrameStartTick, isQuitRequested, renderer, stats) \
    firstprivate(settings, runNumber)
    while (!isQuitRequested && runNumber < settings.maxRunNumber) {
        runNumber++;
#pragma omp master
        {
            currentFrameStartTick = SDL_GetTicks();
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_EVENT_KEY_DOWN: {
                        if (event.key.scancode == SDL_SCANCODE_ESCAPE) isQuitRequested = true;
                        break;
                    }
                    case SDL_EVENT_WINDOW_CLOSE_REQUESTED: {
                        isQuitRequested = true;
                        break;
                    }
                    default: {}
                }
            }

            stats.startRun();
        }
#pragma omp barrier
        updateBoidsVelocities(boids, grid, settings);
        updateBoidsPositions(boids, grid, settings, currentFrameStartTick - lastFrameStartTick);

#pragma omp master
        {
            stats.endRun();

            updateBoidsVertices(boids, renderer, settings);
            SDL_RenderClear(renderer);
            SDL_RenderGeometry(renderer, nullptr, boids.vertices.data(), static_cast<int>(3 * settings.population), nullptr, 0);
            SDL_RenderPresent(renderer);
            lastFrameStartTick = currentFrameStartTick;
        }
    }
    stats.log();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}