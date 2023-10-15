#!/bin/bash

# Check if the build directory does not exist
if [ ! -d "build" ]; then
    # If it doesn't exist, create it and run cmake
    cmake -S . -B build
fi

# Always build the project
cmake --build build