#ifndef UTILITY_H
#define UTILITY_H

#include <chrono>
#include <iostream>

class Timer {
private:
    std::chrono::high_resolution_clock::time_point startTime;

public:
    void start();
    double stop() const;
};

#endif
