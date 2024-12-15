import re

def extract_execution_time(filename):
    with open(filename, 'r') as file:
        for line in file:
            match = re.search(r'Execution: (\d+\.\d+) ms', line)
            if match:
                return float(match.group(1))
    return None

# File paths
serial_output = "serial/serial_output.txt"
parallel_output = "parallel/parallel_output.txt"

# Extract execution times
serial_time = extract_execution_time(serial_output)
parallel_time = extract_execution_time(parallel_output)

if serial_time and parallel_time:
    speedup = serial_time / parallel_time
    print(f"Serial Execution Time: {serial_time:.3f} ms")
    print(f"Parallel Execution Time: {parallel_time:.3f} ms")
    print(f"Speedup: {speedup:.2f}x")
else:
    print("Error: Could not extract execution times.")