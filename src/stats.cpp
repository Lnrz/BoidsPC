#include "stats.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>

Stats::Stats(std::string logFile)
    :logFile{ std::move(logFile) } {}

void Stats::startRun() {
    runNumber++;
    startTime = std::chrono::steady_clock::now();
}

void Stats::endRun() {
    const auto endTime{ std::chrono::steady_clock::now() };
    runTimes.push_back(std::chrono::duration_cast<Duration>(endTime - startTime));
    if (runNumber % 1000 == 0) {
        std::cout << "Reached " << runNumber << "th iteration" << std::endl;
    }
}

void Stats::log() const {
    std::ofstream log{logFile, std::ofstream::app};
    const auto [minIt, maxIt] =
        std::minmax_element(runTimes.cbegin(), runTimes.cend());
    const auto meanRunTime{
        std::accumulate(runTimes.begin(), runTimes.end(), Duration{}) / runNumber
    };
    const auto varRunTime{
        std::accumulate(runTimes.begin(), runTimes.end(), Duration{},
        [meanRunTime](const Duration x, const Duration y) {
            const auto dif{ y - meanRunTime };
            return x + dif * dif.count();
        }) / runNumber
    };
    log << std::format(
        "Date:{0:%F},Time:{0:%R},Runs:{1},Max:{2},Min:{3},Mean:{4},Variance:{5}",
        std::chrono::system_clock::now(), runNumber, *maxIt, *minIt, meanRunTime, varRunTime)
        << std::endl;
}