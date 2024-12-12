#ifndef FILTERS_H
#define FILTERS_H

#include <vector>
#include <string>
#include <iostream>
#include <chrono>

void band_pass_filter(const std::vector<float>& input, std::vector<float>& output);
void notch_filter(const std::vector<float>& input, std::vector<float>& output);
void FIR_filter(const std::vector<float>& input, std::vector<float>& output);
void IIR_filter(const std::vector<float>& input, std::vector<float>& output);

#endif