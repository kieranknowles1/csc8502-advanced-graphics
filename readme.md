# CSC8502 - Advanced Graphics for Games

This repository contains the code for the Advanced Graphics for Games module at Newcastle University as part of the MSc Computer Game Engineering course.

## Controls

In demos with a camera:
- `WASD` movement
- `Space` to move up
- `C` to move down
- Hold `Tab` to move faster
- `Mouse` to look around
- `P` to print the current camera position and orientation

In the `coursework` demo:
- `1`, `2` hold to shift between past/future scenes
- `3` to lock/unlock frustum culling
- `4` to summon a shadow-casting light
- `5` to toggle sun light

All controls use scancodes, so will be in the same place regardless of keyboard layout.

## Building the project

The project is built using CMake and supports Windows and Linux. MacOS may work, but is completely untested.

### Dependencies

The following libraries must be available on `CMAKE_PREFIX_PATH`:
- [SDL2](https://github.com/libsdl-org/SDL)
- OpenGL (included with Visual Studio on Windows)


### Building on Linux

A Nix flake is provided for building on Linux, simply run `nix develop` to enter
a shell and configure CMake. Then run `make` to build the project. This will
work even if breaking changes are made to dependencies, as they are all pinned
in `flake.lock`.

### Building on Windows

By default, CMake will look in `H:/cmake` for libraries, you will probably need to change this to match your system.

The project makes heavy use of `std` containers, which are very slow in MSVC debug builds. Because of this, using `RelWithDebInfo` is recommended for development.

## Third-Party Assets

The following directories contain assets from external sources:

- `Meshes/quaternius`, `Textures/quaternius`: [Stylized Natrue Megakit by Quaternius](https://quaternius.itch.io/stylized-nature-megakit). Used under the [CC0 License](https://creativecommons.org/publicdomain/zero/1.0/) for educational purposes.
- `Textures/crystallotus`: [Skybox Textures by Crystallotus](https://crystallotus.itch.io/skybox-textures). Converted into cubemaps using [Panorama to Cubemap](https://jaxry.github.io/panorama-to-cubemap/).