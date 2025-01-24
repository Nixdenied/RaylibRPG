# RaylibRPG

A simple RTS game project built using Raylib.

## Setup Instructions

### 1. Clone the Repository

To clone the repository along with its submodules, use the following command:

```bash
git clone --recurse-submodules https://github.com/Nixdenied/RaylibRPG
```

### 2. Build Raylib

To build Raylib, you need to run the provided build script first. This will build Raylib.

```bash
./build-raylib.sh
```

This script will:
- Create the `build-linux` directory inside the `libs/raylib` folder.
- Build Raylib using `cmake` and `make`.

### 3. Build the Project

Once Raylib is built, you can proceed to build the game project. Follow these steps:

1. Create a `build` directory:

```bash
mkdir build
```

2. Navigate into the `build` directory:

```bash
cd build
```

3. Run `cmake` to configure the project and build it for your platform:

```bash
cmake .. -DBUILD_FOR_LINUX=ON -DBUILD_FOR_WINDOWS=OFF
```

4. Build the project using `make`:

```bash
make
```

### 4. Running the Game

Once the build is complete, you can run the game executable located in the `build` directory.

