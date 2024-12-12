#include "filters.h"
#include <cmath>
#include <thread>

void band_pass_filter(const std::vector<float>& input, std::vector<float>& output) {
    double delta_f = 0.2;
    for (size_t i = 0; i < input.size(); ++i) {
        double f = input[i];
        double filter_response = (f * f) / (f * f + delta_f * delta_f);
        output[i] = f * filter_response;
    }
}

void notch_filter(const std::vector<float>& input, std::vector<float>& output) {
    float f0 = 50;
    int n = 2;
    for (size_t i = 0; i < input.size(); ++i) {
        float f = input[i];
        float ratio = f / f0;
        float H = 1.0f / (std::pow(ratio, 2 * n) + 1.0f);
        output[i] = H;
    }
}

void FIR_filter(const std::vector<float>& input, std::vector<float>& output) {
    static std::vector<float> h = {0.2f, 0.2f, 0.2f, 0.2f, 0.2f};
    int M = h.size();

    for (size_t n = 0; n < input.size(); ++n) {
        output[n] = 0.0f;
        for (int k = 0; k < M; ++k) {
            if (n >= k) {
                output[n] += h[k] * input[n - k];
            }
        }
    }
}

void IIR_filter(const std::vector<float>& input, std::vector<float>& output) {
    static std::vector<float> a = {1.0f, -0.8f, 1.0f, -0.8f, 1.0f, -0.8f};
    static std::vector<float> b = {0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f};
    int M = a.size();
    int N = b.size();

    for (size_t n = 0; n < input.size(); ++n) {

        for (int k = 0; k < N; ++k) {
            if (n >= k) {
                output[n] += b[k] * input[n - k];
            }
        }

        for (int j = 1; j < M; ++j) { 
            if (n >= j) {
                output[n] -= a[j] * output[n - j];
            }
        }
    }
}