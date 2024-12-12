#ifndef FILTERS_H
#define FILTERS_H

#include <vector>
#include <string>
#include <iostream>
#include <chrono>

void* band_pass_filter(void* arg);
void* notch_filter(void* arg);
void* FIR_filter(void* arg);
void* IIR_filter(void* input);

void applyFilter(const std::string& filterName, const std::vector<float>& input, std::vector<float>& output);

#endif