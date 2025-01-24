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

if [ -f "${RAYLIB_DIR}/build-linux/raylib/libraylib.a" ]; then
    mv ${RAYLIB_DIR}/build-linux/raylib/libraylib.a ${RAYLIB_DIR}/build-linux/libraylib.a
fi

# Inform the user
echo "Raylib has been built, and the library has been moved to ${RAYLIB_DIR}/build-linux/libraylib.a"

