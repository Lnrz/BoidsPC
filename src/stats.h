#ifndef BOIDS_STATS_H
#define BOIDS_STATS_H

#include <cstdint>
#include <string>
#include <chrono>

// Simple class to register stats across runs.
class Stats {
public:
    using Duration = std::chrono::microseconds;

    //"logFile" is the path of the file in which the stats will be appended.
    explicit Stats(std::string logFile);

    // Start registering time of run.
    // Should be called only once at the start of the run.
    void startRun();
    // End registering time of run .
    // Print a message every 1000 runs.
    // Should be called only once at the end of the run.
    void endRun();

    // Write to "logFile" the stats of the runs:
    // date, time, runs, max, min, mean, variance
    void log() const;

private:
    std::string logFile{};
    uint32_t runNumber{};
    std::chrono::time_point<std::chrono::steady_clock> startTime{};
    std::vector<Duration> runTimes{};
};

#endif //BOIDS_STATS_H