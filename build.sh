#!/bin/bash

# Check if the Debug build directory does not exist
if [ ! -d "build" ]; then
    # If it doesn't exist, create it and run cmake for Debug
    cmake -S . -B build/
fi



# Build the Debug version
cmake --build build --config Debug

# Build the Release version
cmake --build build --config Release
