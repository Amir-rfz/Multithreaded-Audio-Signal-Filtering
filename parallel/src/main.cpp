#include <iostream>
#include <sndfile.h>
#include <vector>
#include <string>
#include <cstring>
#include <atomic>
#include <unistd.h>
#include "filters.h"
#include "utility.h"
#include "thread.hpp"

using namespace std;

void readWavFile(const std::string& inputFile, std::vector<float>& data, SF_INFO& fileInfo) {
    SNDFILE* inFile = sf_open(inputFile.c_str(), SFM_READ, &fileInfo);
    if (!inFile) {
        std::cerr << "Error opening input file: " << sf_strerror(NULL) << std::endl;
        exit(1);
    }

    data.resize(fileInfo.frames * fileInfo.channels);
    sf_count_t numFrames = sf_readf_float(inFile, data.data(), fileInfo.frames);
    if (numFrames != fileInfo.frames) {
        std::cerr << "Error reading frames from file." << std::endl;
        sf_close(inFile);
        exit(1);
    }

    sf_close(inFile);
    std::cout << "Successfully read " << numFrames << " frames from " << inputFile << std::endl;
}

void writeWavFile(const std::string& outputFile, const std::vector<float>& data,  SF_INFO& fileInfo) {
    sf_count_t originalFrames = fileInfo.frames;
    SNDFILE* outFile = sf_open(outputFile.c_str(), SFM_WRITE, &fileInfo);
    if (!outFile) {
        std::cerr << "Error opening output file: " << sf_strerror(NULL) << std::endl;
        exit(1);
    }

    sf_count_t numFrames = sf_writef_float(outFile, data.data(), originalFrames);
    if (numFrames != originalFrames) {
        std::cerr << "Error writing frames to file." << std::endl;
        sf_close(outFile);
        exit(1);
    }

    sf_close(outFile);
    std::cout << "Successfully wrote " << numFrames << " frames to " << outputFile << std::endl;
}

float sumVector(const std::vector<float>& values) {
    float sum = 0.0f;
    for (float value : values) {
        sum += value;
    }
    return sum;
}

int main(int argc, char* argv[]){
    if (argc < 2){
        std::cerr << "Usage: " << argv[0] << " <input WAV file>" << std::endl;
        return 1;
    }

    std::string inputFile = argv[1];
    
    std::string BP_outputFile = "res/BP_filter.wav";
    std::string N_outputFile = "res/N_filter.wav";
    std::string FIR_outputFile = "res/FIR_filter.wav";
    std::string IRR_outputFile = "res/IRR_filter.wav";

    SF_INFO fileInfo, BP_fileInfo, N_fileInfo, FIR_fileInfo, IRR_fileInfo;
    std::vector<float> audioData;

    Timer timer, execution_timer;
    double read_duration, write_duration, execution_duration, BP_duration, N_duration, FIR_duration, IIR_duration;


    std::memset(&fileInfo, 0, sizeof(fileInfo));

    execution_timer.start();
    timer.start();
    
    readWavFile(inputFile, audioData, fileInfo);
    BP_fileInfo = fileInfo;
    N_fileInfo = fileInfo;
    FIR_fileInfo = fileInfo;
    IRR_fileInfo = fileInfo;
    read_duration = timer.stop();
    
    std::vector<float> BP_filteredData(audioData.size(), 0.0f);
    std::vector<float> N_filteredData(audioData.size(), 0.0f);
    std::vector<float> IRR_filteredData(audioData.size(), 0.0f);
    std::vector<float> FIR_filteredData(audioData.size(), 0.0f);

    std::vector<std::atomic<bool>> ready_flags(audioData.size());
    for (size_t i = 0; i < audioData.size(); ++i) {
        ready_flags[i].store(false, std::memory_order_relaxed);
    }

    timer.start();
    create_threads(band_pass_filter ,audioData, BP_filteredData, ready_flags);
    BP_duration = timer.stop();
    
    timer.start();
    create_threads(notch_filter ,audioData, N_filteredData, ready_flags);
    N_duration = timer.stop();

    timer.start();
    create_threads(FIR_filter ,audioData, FIR_filteredData, ready_flags);
    FIR_duration = timer.stop();

    timer.start();

    // FIRST WAY
    // create_threads(IIR_filter ,audioData, IRR_filteredData, ready_flags);

    // SECOND WAY
    ThreadArgs* args = new ThreadArgs(1 ,audioData, IRR_filteredData, ready_flags);
    IIR_filter(args);

    IIR_duration = timer.stop();

    timer.start();
    writeWavFile(BP_outputFile, BP_filteredData, BP_fileInfo);
    writeWavFile(N_outputFile, N_filteredData, N_fileInfo);
    writeWavFile(FIR_outputFile, FIR_filteredData, FIR_fileInfo);
    writeWavFile(IRR_outputFile, IRR_filteredData, IRR_fileInfo);
    write_duration = timer.stop();

    execution_duration = execution_timer.stop();

    std::cout << "Read: " << read_duration << " ms" << std::endl;
    std::cout << "write: " << write_duration << " ms" << std::endl;
    std::cout << "Band-pass Filter" << ": " << BP_duration << " ms" << std::endl;
    std::cout << "Notch Filter" << ": " << N_duration << " ms" << std::endl;
    std::cout << "FIR Filter" << ": " << FIR_duration << " ms" << std::endl;
    std::cout << "IRR Filter" << ": " << IIR_duration << " ms" << std::endl;
    std::cout << "Execution: " << execution_duration << " ms" << std::endl;

    // UNCOMMENT IF YOU WANT TO CHECK THE VALID OUTPUT
    
    // std::cout << "sum of input Data :" << sumVector(audioData) << std::endl;
    // std::cout << "sum of BP filtered Data :" << sumVector(BP_filteredData) << std::endl;
    // std::cout << "sum of N filtered Data :" << sumVector(N_filteredData) << std::endl;
    // std::cout << "sum of FIR filtered Data :" << sumVector(FIR_filteredData) << std::endl;
    // std::cout << "sum of IRR filtered Data :" << sumVector(IRR_filteredData) << std::endl;

    return 0;
}
