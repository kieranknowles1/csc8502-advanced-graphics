# CSC8502 - Advanced Graphics for Games

This repository contains the code for the Advanced Graphics for Games module at Newcastle University as part of the MSc Computer Game Engineering course.

## Building the project

The project is built using CMake and supports Windows and Linux. MacOS may work, but is completely untested.

### Dependencies

The following libraries must be available on `CMAKE_PREFIX_PATH`:
- SDL2
- OpenGL

By default, this path is set to `H:/cmake`, and will probably need to be changed to match your system.

### Building on Linux

A Nix flake is provided for building on Linux, simply run `nix develop` to enter
a shell and configure CMake. Then run `make` to build the project.
