#ifndef __THREAD_HPP
#define __THREAD_HPP

#include <vector>
#include <pthread.h>
#include <iostream>
#include <atomic>
#define NUMBER_OF_THREADS 6

struct ThreadArgs
{
    ThreadArgs(long _tid, const std::vector<float>& input_audio, std::vector<float>& output_audio, std::vector<std::atomic<bool>>& ready_flags) :
    tid(_tid), input_audio(input_audio), output_audio(output_audio), ready_flags(ready_flags){}
    long tid;
    const std::vector<float>& input_audio;
    std::vector<float>& output_audio;
    std::vector<std::atomic<bool>>& ready_flags;
};

void create_threads(void *(*func)(void *), const std::vector<float>& input_audio, std::vector<float>& output_audio, std::vector<std::atomic<bool>>& ready_flags);
#endif