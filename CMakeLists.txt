cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(RaylibRPG VERSION 1.0 LANGUAGES C)

# Specify the C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# Options for cross-compiling to different targets
option(BUILD_FOR_WINDOWS "Build the project for Windows using MinGW" OFF)
option(BUILD_FOR_LINUX "Build the project for Linux" ON)

# Set Raylib paths within the project
set(RAYLIB_DIR ${CMAKE_SOURCE_DIR}/libs/raylib)
set(RAYLIB_INCLUDE_DIR ${RAYLIB_DIR}/src)
set(RAYLIB_LIB_DIR_LINUX ${RAYLIB_DIR}/build-linux)
set(RAYLIB_LIB_DIR_WINDOWS ${RAYLIB_DIR}/build-windows)

# Set Platform-Specific Compiler and Linker Flags
if(BUILD_FOR_WINDOWS)
    set(CMAKE_SYSTEM_NAME Windows)
    set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
    set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
    set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)

    # Include the raylib headers (for Windows)
    include_directories(${RAYLIB_INCLUDE_DIR})

    # Additional Windows Libraries
    set(WINDOWS_LIBS gdi32 winmm)
    
    # Disable shared linking for MinGW
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static -static-libgcc -static-libstdc++")

elseif(BUILD_FOR_LINUX)
    # Default settings for Linux
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)

    # Include the raylib headers (for Linux)
    include_directories(${RAYLIB_INCLUDE_DIR})
endif()

# Include directories for the project
include_directories(${CMAKE_SOURCE_DIR}/src/scenes)
include_directories(${CMAKE_SOURCE_DIR}/src/core)
include_directories(${CMAKE_SOURCE_DIR}/src/utils)

# Source files
file(GLOB_RECURSE SCENE_SOURCES src/scenes/*.c)
file(GLOB_RECURSE CORE_SOURCES src/core/*.c)
file(GLOB_RECURSE UTILS_SOURCES src/utils/*.c)

set(SOURCES
    src/main.c
    ${SCENE_SOURCES}
    ${CORE_SOURCES}
    ${UTILS_SOURCES}
)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link against Raylib and other dependencies (Static)
if(BUILD_FOR_WINDOWS)
    # Link against the cross-compiled raylib library for Windows
    target_link_libraries(${PROJECT_NAME} PRIVATE ${RAYLIB_LIB_DIR_WINDOWS}/raylib/libraylib.a ${WINDOWS_LIBS})
elseif(BUILD_FOR_LINUX)
    # Link against the Raylib library for Linux
    target_link_libraries(${PROJECT_NAME} PRIVATE ${RAYLIB_LIB_DIR_LINUX}/raylib/libraylib.a m pthread)
endif()

# Set debug flags if in Debug mode
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -fsanitize=address")

# Optionally enable optimizations for Release mode
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2")

