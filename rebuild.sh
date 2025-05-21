#!/bin/bash

# Exit on error
set -e

# Navigate to the project directory
cd "$(dirname "$0")"

# Clean build directory
rm -rf build
mkdir -p build
cd build

# Run CMake configuration
echo "Configuring project with CMake..."
cmake .. -DCYCLONEDDS_HOME=/usr/local/cyclonedds

# Build the project
echo "Building project..."
cmake --build .

echo "Build completed successfully!"
