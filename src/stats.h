#ifndef BOIDS_STATS_H
#define BOIDS_STATS_H

#include <cstdint>
#include <string>
#include <chrono>

class Stats {
public:
    using Duration = std::chrono::microseconds;

    explicit Stats(std::string logFile);

    void startRun();
    void endRun();

    void log() const;

private:
    std::string logFile{};
    uint32_t runNumber{};
    std::chrono::time_point<std::chrono::steady_clock> startTime{};
    std::vector<Duration> runTimes{};
};

#endif //BOIDS_STATS_H