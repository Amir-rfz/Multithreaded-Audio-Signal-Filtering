#include <iostream>
#include <sndfile.h>
#include <vector>
#include <string>
#include <cstring>
#include "filters.h"
#include "utility.h"
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
    std::vector<float> BP_filteredData, N_filteredData, IRR_filteredData, FIR_filteredData;

    Timer read_timer, execution_timer;
    double read_duration, execution_duration;

    std::memset(&fileInfo, 0, sizeof(fileInfo));

    execution_timer.start();
    read_timer.start();
    readWavFile(inputFile, audioData, fileInfo);
    BP_fileInfo = fileInfo;
    N_fileInfo = fileInfo;
    FIR_fileInfo = fileInfo;
    IRR_fileInfo = fileInfo;
    read_duration = read_timer.stop();
    std::cout << "Read: " << read_duration << " ms" << std::endl;

    applyFilter("Band-pass Filter", audioData, BP_filteredData);
    writeWavFile(BP_outputFile, BP_filteredData, BP_fileInfo);
    
    applyFilter("Notch Filter", audioData, N_filteredData);
    writeWavFile(N_outputFile, N_filteredData, N_fileInfo);

    applyFilter("FIR Filter", audioData, FIR_filteredData);
    writeWavFile(FIR_outputFile, FIR_filteredData, FIR_fileInfo);

    applyFilter("IRR Filter", audioData, IRR_filteredData);
    writeWavFile(IRR_outputFile, IRR_filteredData, IRR_fileInfo);

    execution_duration = execution_timer.stop();
    std::cout << "Execution: " << execution_duration << " ms" << std::endl;
    
    return 0;
}
