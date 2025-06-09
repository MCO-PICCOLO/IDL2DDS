#!/bin/bash

# Build script for cyclonedds-sender

# Function to display usage
show_usage() {
    echo "Usage: $0 [options]"
    echo "Options:"
    echo "  -h, --help      Show this help message"
    echo "  -d, --docker    Build using Docker"
    echo "  -r, --run       Run after building"
    echo "  -c, --clean     Clean build directory before building"
}

# Default values
USE_DOCKER=0
RUN_AFTER_BUILD=0
CLEAN_BUILD=0

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -d|--docker)
            USE_DOCKER=1
            shift
            ;;
        -r|--run)
            RUN_AFTER_BUILD=1
            shift
            ;;
        -c|--clean)
            CLEAN_BUILD=1
            shift
            ;;
        -h|--help)
            show_usage
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            show_usage
            exit 1
            ;;
    esac
done

# Build with Docker
build_with_docker() {
    echo "Building with Docker..."
    docker build -t cyclonedds-sender .
    
    if [ $RUN_AFTER_BUILD -eq 1 ]; then
        echo "Running container..."
        docker run --rm cyclonedds-sender
    else
        echo "Docker image built successfully."
        echo "Run with: docker run --rm cyclonedds-sender"
    fi
}

# Build locally
build_local() {
    echo "Building locally..."
    
    # Run scripts to ensure newlines and generate handlers
    if [ -f "./ensure_newlines.sh" ]; then
        echo "Fixing newlines in IDL files..."
        chmod +x ./ensure_newlines.sh
        ./ensure_newlines.sh
    fi
    
    if [ -f "./auto_generate_handlers.sh" ]; then
        echo "Auto-generating handlers for IDL files..."
        chmod +x ./auto_generate_handlers.sh
        ./auto_generate_handlers.sh
    fi
    
    # Create build directory if it doesn't exist
    mkdir -p build
    
    # Clean if requested
    if [ $CLEAN_BUILD -eq 1 ]; then
        echo "Cleaning build directory..."
        rm -rf build/*
    fi
    
    # Change to build directory
    cd build
    
    # Run CMake with CycloneDDS path if provided
    if [ -n "$CYCLONEDDS_HOME" ]; then
        echo "Using CYCLONEDDS_HOME: $CYCLONEDDS_HOME"
        cmake .. -DCYCLONEDDS_HOME=$CYCLONEDDS_HOME
    else
        echo "CYCLONEDDS_HOME not set, using default system paths"
        cmake ..
    fi
    
    # Build the project
    cmake --build .
    
    # Check if build was successful
    if [ $? -eq 0 ]; then
        echo "Build successful!"
        
        if [ $RUN_AFTER_BUILD -eq 1 ]; then
            echo "Running application..."
            ./cyclonedds_sender
        else
            echo "Run with: ./build/cyclonedds_sender"
        fi
    else
        echo "Build failed!"
        exit 1
    fi
}

# Main execution
if [ $USE_DOCKER -eq 1 ]; then
    build_with_docker
else
    build_local
fi

exit 0
