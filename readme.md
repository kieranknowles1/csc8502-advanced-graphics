# CSC8502 - Advanced Graphics for Games

This repository contains the code for the Advanced Graphics for Games module at Newcastle University as part of the MSc Computer Game Engineering course.

## Controls

In demos with a camera:
- `WASD` movement
- `Space` to move up
- `C` to move down
- Hold `Tab` to move faster
- `Mouse` to look around
- `P` to print the current camera position

## Building the project

The project is built using CMake and supports Windows and Linux. MacOS may work, but is completely untested.

### Dependencies

The following libraries must be available on `CMAKE_PREFIX_PATH`:
- [SDL2](https://github.com/libsdl-org/SDL)
- OpenGL (included with Visual Studio on Windows)

By default, this path is set to `H:/cmake`, and will probably need to be changed to match your system.

### Building on Linux

A Nix flake is provided for building on Linux, simply run `nix develop` to enter
a shell and configure CMake. Then run `make` to build the project. This will
work even if breaking changes are made to dependencies, as they are all pinned
in `flake.lock`.

### Building on Windows

The project makes heavy use of `std` containers, which are very slow in MSVC debug builds. Because of this, using `RelWithDebInfo` is recommended for development.