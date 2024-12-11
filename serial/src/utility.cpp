#include "utility.h"

void Timer::start() {
    startTime = std::chrono::high_resolution_clock::now();
}

double Timer::stop() const {
    auto endTime = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(endTime - startTime).count();
}
