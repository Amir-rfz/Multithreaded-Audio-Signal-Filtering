#ifndef FILTERS_H
#define FILTERS_H

#include <vector>
#include <string>
#include <iostream>
#include <chrono>

std::vector<float>& band_pass_filter(const std::vector<float>& input);
std::vector<float>& notch_filter(const std::vector<float>& input);
std::vector<float>& FIR_filter(const std::vector<float>& input);
std::vector<float>& IIR_filter(const std::vector<float>& input);

void applyFilter(const std::string& filterName, const std::vector<float>& input, std::vector<float>& output);

#endif