#!/bin/bash
# filepath: /home/edo/2025/projects/dds-sender/IDL2DDS/run_test.sh

# Check if test file is provided
if [ $# -lt 1 ]; then
    echo "Usage: $0 <test-file.yaml>"
    echo "Example: $0 test-message.yaml"
    exit 1
fi

TEST_FILE="$1"

# Ensure build directory exists
if [ ! -d "build" ]; then
    mkdir -p build
fi

# Build the project
cd build
cmake ..
make -j$(nproc)
cd ..

# Run the test
echo "Running test with configuration: $TEST_FILE"
./build/IDL2DDS "$TEST_FILE"