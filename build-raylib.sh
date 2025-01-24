#!/bin/bash

# Set the Raylib directory
RAYLIB_DIR="./libs/raylib"

# Create the build-linux directory inside raylib if it doesn't exist
mkdir -p ${RAYLIB_DIR}/build-linux

# Navigate to the raylib build directory
cd ${RAYLIB_DIR}/build-linux

# Run cmake to configure and build raylib
cmake ..
make

echo "Raylib has been built"

