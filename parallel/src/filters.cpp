#include "filters.h"
#include <cmath>
#include <thread>
#include <atomic>
#include <unistd.h>
#include "thread.hpp"
#include <random>
#include <mutex>
#include <condition_variable>

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

    std::vector<float> h = {
        0.002134f,  0.003579f,  0.005801f,  0.008785f,  0.012506f,  0.016920f,  0.021973f,  
        0.027592f,  0.033691f,  0.040174f,  0.046931f,  0.053843f,  0.060784f,  0.067620f,  
        0.074219f,  0.080449f,  0.086183f,  0.091299f,  0.095681f,  0.099217f,  0.101808f,  
        0.103368f,  0.103828f,  0.103138f,  0.101264f,  0.098189f,  0.093917f,  0.088471f,  
        0.081891f,  0.074240f,  0.065598f,  0.056056f,  0.045720f,  0.034708f,  0.023145f,  
        0.011159f, -0.001089f, -0.013442f, -0.025739f, -0.037821f, -0.049530f, -0.060711f,  
        -0.071216f, -0.080906f, -0.089655f, -0.097343f, -0.103872f, -0.109156f, -0.113123f
    };
    int M = h.size();

    size_t chunk_size = args->input_audio.size() / NUMBER_OF_THREADS;
    size_t start = args->tid * chunk_size;
    size_t end = (args->tid == NUMBER_OF_THREADS - 1) ? args->input_audio.size() : (args->tid + 1) * chunk_size;

    for (size_t n = start; n < end; ++n) {
        float sum = 0.0f;
        for (int k = 0; k < M; ++k) {
            if (n >= k) {
                sum += h[k] * args->input_audio[n - k];
            }
        }
        args->output_audio[n] = sum;
    }

    return nullptr;
}

// void* IIR_filter(void* arg) {
//     ThreadArgs* args = static_cast<ThreadArgs*>(arg);

//     // Filter coefficients
//     static std::vector<float> a = {
//         1.0f, -0.95f, 0.85f, -0.75f, 0.65f, -0.55f, 0.45f, -0.35f, 0.25f, -0.15f, 0.05f,
//         -0.03f, 0.01f, -0.008f, 0.005f, -0.003f, 0.001f, -0.0008f, 0.0005f, -0.0003f, 0.0001f
//     };

//     static std::vector<float> b = {
//         0.01f, 0.015f, 0.02f, 0.03f, 0.04f, 0.05f, 0.06f, 0.07f, 0.08f, 0.09f, 0.1f,
//         0.09f, 0.08f, 0.07f, 0.06f, 0.05f, 0.04f, 0.03f, 0.02f, 0.015f, 0.01f
//     };

//     int M = a.size();
//     int N = b.size();

//     size_t chunk_size = args->input_audio.size() / NUMBER_OF_THREADS;
//     size_t start = args->tid * chunk_size;
//     size_t end = (args->tid == NUMBER_OF_THREADS - 1) ? args->input_audio.size() : (args->tid + 1) * chunk_size;

//     for (size_t n = start; n < end; ++n) {

//         for (int k = 0; k < N; ++k) {
//             if (n >= k) {
//                 args->output_audio[n] += b[k] * args->input_audio[n - k];
//             }
//         }

//         for (int j = 1; j < M; ++j) {
//             if (n >= j) {
//                 size_t dependency_index = n - j;

//                 while (!args->ready_flags[dependency_index].load(std::memory_order_acquire)) {
//                     usleep(10); //for busy waiting
//                 }

//                 args->output_audio[n] -= a[j] * args->output_audio[dependency_index];
//             }
//         }

//         args->ready_flags[n].store(true, std::memory_order_release);
//     }

//     return nullptr;
// }

void* IIR_filter(void* arg) {
    ThreadArgs* args = static_cast<ThreadArgs*>(arg);

    static std::vector<float> a = {
        1.0f, -0.95f, 0.85f, -0.75f, 0.65f, -0.55f, 0.45f, -0.35f, 0.25f, -0.15f, 0.05f,
        -0.03f, 0.01f, -0.008f, 0.005f, -0.003f, 0.001f, -0.0008f, 0.0005f, -0.0003f, 0.0001f
    };

    std::vector<float> b = {
        0.002134f,  0.003579f,  0.005801f,  0.008785f,  0.012506f,  0.016920f,  0.021973f,  
        0.027592f,  0.033691f,  0.040174f,  0.046931f,  0.053843f,  0.060784f,  0.067620f,  
        0.074219f,  0.080449f,  0.086183f,  0.091299f,  0.095681f,  0.099217f,  0.101808f,  
        0.103368f,  0.103828f,  0.103138f,  0.101264f,  0.098189f,  0.093917f,  0.088471f,  
        0.081891f,  0.074240f,  0.065598f,  0.056056f,  0.045720f,  0.034708f,  0.023145f,  
        0.011159f, -0.001089f, -0.013442f, -0.025739f, -0.037821f, -0.049530f, -0.060711f,  
        -0.071216f, -0.080906f, -0.089655f, -0.097343f, -0.103872f, -0.109156f, -0.113123f
    };

    int M = a.size();
    int N = b.size();

    create_threads(FIR_filter ,args->input_audio, args->output_audio, args->ready_flags);
    
    for (size_t n = 0; n < args->input_audio.size() ; ++n) {
        for (int j = 1; j < M; ++j) {
            if (n >= j) {
                args->output_audio[n] -= a[j] * args->output_audio[n -j];
            }
        }
    }

    return nullptr;
}