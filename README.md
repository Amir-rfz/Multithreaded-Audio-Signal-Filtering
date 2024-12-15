# Multithreaded Audio Signal Filtering

**Multithreaded Audio Signal Filtering** is a project that applies digital filters to audio signals, including **Band-Pass**, **Notch**, **FIR**, and **IIR** filters. The project uses the **libsndfile** library to process **WAV** audio files and demonstrates both serial and multithreaded filtering techniques.

## Features

- **Band-Pass Filter**: Allows frequencies within a specific range to pass through.
- **Notch Filter**: Removes a specific frequency while leaving others intact.
- **FIR (Finite Impulse Response) Filter**: Provides a stable, non-recursive filtering solution.
- **IIR (Infinite Impulse Response) Filter**: Uses feedback for an efficient, recursive filter.

## Installation

### Prerequisites

- **C++11** or later
- **libsndfile** library
- **g++** compiler (or any C++11 compatible compiler)

### Steps

1. Clone this repository:
    ```bash
    git clone https://github.com/<YourUsername>/Multithreaded-Audio-Signal-Filtering.git
    cd Multithreaded-Audio-Signal-Filtering
    ```

2. Install the **libsndfile** library:
    - On Ubuntu:
      ```bash
      sudo apt-get install libsndfile1-dev
      ```
    - On macOS:
      ```bash
      brew install libsndfile
      ```

3. Compile the project:
    ```bash
    g++ -std=c++11 -o AudioFilters main.cpp -lsndfile -pthread
    ```

4. Run the program:
    ```bash
    ./AudioFilters input.wav output.wav
    ```

   Replace `input.wav` with your source audio file and `output.wav` with the desired output file name.

## Performance

The project compares the **serial** and **multithreaded** implementations of the filtering process. It evaluates the **speedup** gained through multithreading, demonstrating performance improvements in real-time audio processing.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- **libsndfile** library for audio file handling: [https://github.com/libsndfile/libsndfile](https://github.com/libsndfile/libsndfile)
- Inspired by digital signal processing concepts and parallel computing techniques.

