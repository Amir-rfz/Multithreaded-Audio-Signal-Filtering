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