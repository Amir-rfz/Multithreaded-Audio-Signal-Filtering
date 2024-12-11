#include "filters.h"
#include <cmath>
#include <thread>

std::vector<float>& band_pass_filter(const std::vector<float>& input) {
    static std::vector<float> output(input.size());
    double delta_f = 0.2;
    for (size_t i = 0; i < input.size(); ++i) {
        double f = input[i];
        double filter_response = (f * f) / (f * f + delta_f * delta_f);
        output[i] = f * filter_response;
    }
    return output;
}

std::vector<float>& notch_filter(const std::vector<float>& input) {
    static std::vector<float> output(input.size());
    float f0 = 50;
    int n = 2;
    for (size_t i = 0; i < input.size(); ++i) {
        float f = input[i];
        float ratio = f / f0;
        float H = 1.0f / (std::pow(ratio, 2 * n) + 1.0f);
        output[i] = H;
    }
    return output;
}

std::vector<float>& FIR_filter(const std::vector<float>& input) {
    static std::vector<float> output(input.size());
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
    return output;
}

std::vector<float>& IIR_filter(const std::vector<float>& input) {
    static std::vector<float> output(input.size(), 0.0f);
    static std::vector<float> a = {1.0f, -0.8f};
    static std::vector<float> b = {0.5f, 0.5f};
    int M = a.size();
    int N = b.size();

    for (size_t n = 0; n < input.size(); ++n) {
        output[n] = 0.0f;

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

    return output;
}

void applyFilter(const std::string& filterName, const std::vector<float>& input, std::vector<float>& output) {
    auto start = std::chrono::high_resolution_clock::now();

    if (filterName == "Band-pass Filter")
        output = band_pass_filter(input);
    else if (filterName == "Notch Filter")
        output = notch_filter(input);
    else if (filterName == "IRR Filter")
        output = IIR_filter(input);
    else if (filterName == "FIR Filter")
        output = FIR_filter(input);

    auto end = std::chrono::high_resolution_clock::now();
    double durationMs = std::chrono::duration<double, std::milli>(end - start).count();

    std::cout << filterName << ": " << durationMs << " ms" << std::endl;
}