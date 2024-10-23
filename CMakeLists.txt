cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(RaylibRPG VERSION 1.0 LANGUAGES C)

# Specify the C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# Include the raylib headers
include_directories(/opt/git/RayLibRPG/libs/raylib/src)
include_directories(${CMAKE_SOURCE_DIR}/src/scenes)
include_directories(${CMAKE_SOURCE_DIR}/src/core)

# Source files
file(GLOB_RECURSE SCENE_SOURCES src/scenes/*.c)
file(GLOB_RECURSE CORE_SOURCES src/core/*.c)

set(SOURCES
    src/main.c
    ${SCENE_SOURCES}
    ${CORE_SOURCES}
)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link against the pre-built raylib library
target_link_libraries(${PROJECT_NAME} /opt/git/RayLibRPG/libs/raylib/build/raylib/libraylib.a m)
        
# Set debug flags if in Debug mode
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -fsanitize=address")

# Optionally enable optimizations for Release mode
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2")
