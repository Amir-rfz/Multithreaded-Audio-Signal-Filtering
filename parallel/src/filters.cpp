#include "filters.h"
#include <cmath>
#include <thread>
#include <atomic>
#include <unistd.h>
#include "thread.hpp"

void* band_pass_filter(void* arg) {
    ThreadArgs* args = static_cast<ThreadArgs*>(arg);

    double delta_f = 0.2;

    size_t chunk_size = args->input_audio.size() / NUMBER_OF_THREADS;
    size_t start = args->tid * chunk_size;
    size_t end = (args->tid == NUMBER_OF_THREADS - 1) ? args->input_audio.size() : (args->tid + 1) * chunk_size;

    for (size_t i = start; i < end; ++i) {
        double f = args->input_audio[i];
        double filter_response = (f * f) / (f * f + delta_f * delta_f);
        args->output_audio[i] = f * filter_response;
    }

    return nullptr;
}

void* notch_filter(void* arg) {
    ThreadArgs* args = static_cast<ThreadArgs*>(arg);

    float f0 = 50;
    int n = 2;

    size_t chunk_size = args->input_audio.size() / NUMBER_OF_THREADS;
    size_t start = args->tid * chunk_size;
    size_t end = (args->tid == NUMBER_OF_THREADS - 1) ? args->input_audio.size() : (args->tid + 1) * chunk_size;

    for (size_t i = start; i < end; ++i) {
        float f = args->input_audio[i];
        float ratio = f / f0;
        float H = 1.0f / (std::pow(ratio, 2 * n) + 1.0f);
        args->output_audio[i] = H;
    }
    return nullptr;
}


void* FIR_filter(void* arg){
    ThreadArgs* args = static_cast<ThreadArgs*>(arg);

    std::vector<float> h = {0.2f, 0.2f, 0.2f, 0.2f, 0.2f};
    int M = h.size();

    size_t chunk_size = args->input_audio.size() / NUMBER_OF_THREADS;
    size_t start = args->tid * chunk_size;
    size_t end = (args->tid == NUMBER_OF_THREADS - 1) ? args->input_audio.size() : (args->tid + 1) * chunk_size;

    for (size_t n = start; n < end; ++n) {
        args->output_audio[n] = 0.0f;
        for (int k = 0; k < M; ++k) {
            if (n >= k) {
                args->output_audio[n] += h[k] * args->input_audio[n - k];
            }
        }
    }

    return nullptr;
}

// void* IIR_filter(void* arg) {
//     ThreadArgs* args = static_cast<ThreadArgs*>(arg);

//     static std::vector<float> a = {1.0f, -0.8f};
//     static std::vector<float> b = {0.5f, 0.5f};
//     int M = a.size();
//     int N = b.size();

//     size_t chunk_size = args->input_audio.size() / NUMBER_OF_THREADS;
//     size_t start = args->tid * chunk_size;
//     size_t end = (args->tid == NUMBER_OF_THREADS - 1) ? args->input_audio.size() : (args->tid + 1) * chunk_size;

//     float prev_output[M] = {0.0f};
//     if (args->tid > 0) {
//         size_t boundary_start = start - M;
//         for (int j = 1; j < M; ++j) {
//             if (boundary_start + j < args->input_audio.size()) {
//                 prev_output[j] = args->output_audio[boundary_start + j];
//             }
//         }
//     }

//     for (size_t n = start; n < end; ++n){
//         args->output_audio[n] = 0.0f;

//         for (int k = 0; k < N; ++k) {
//             if (n >= k) {
//                 args->output_audio[n] += b[k] * args->input_audio[n - k];
//             }
//         }

//         for (int j = 1; j < M; ++j) {
//             if (n >= j) {
//                 if (n - j >= start) {
//                     args->output_audio[n] -= a[j] * args->output_audio[n - j];
//                 } else {
//                     args->output_audio[n] -= a[j] * prev_output[j];
//                 }
//             }
//         }
//     }

//     return nullptr;
// }

void* IIR_filter(void* arg) {
    ThreadArgs* args = static_cast<ThreadArgs*>(arg);

    static const std::vector<float> a = {1.0f, -0.8f};
    static const std::vector<float> b = {0.5f, 0.5f};
    int M = a.size();
    int N = b.size();

    size_t chunk_size = args->input_audio.size() / NUMBER_OF_THREADS;
    size_t start = args->tid * chunk_size;
    size_t end = (args->tid == NUMBER_OF_THREADS - 1) 
                 ? args->input_audio.size() 
                 : (args->tid + 1) * chunk_size;

    for (size_t n = start; n < end; ++n) {
        args->output_audio[n] = 0.0f;

        for (int k = 0; k < N; ++k) {
            if (n >= k) {
                args->output_audio[n] += b[k] * args->input_audio[n - k];
            }
        }

        for (int j = 1; j < M; ++j) {
            if (n >= j) {
                size_t dependency_index = n - j;

                while (!args->ready_flags[dependency_index].load(std::memory_order_acquire)) {
                    usleep(10); //for busy waiting
                }

                args->output_audio[n] -= a[j] * args->output_audio[dependency_index];
            }
        }

        args->ready_flags[n].store(true, std::memory_order_release);
    }

    return nullptr;
}