# CSC8502 - Advanced Graphics for Games

- [CSC8502 - Advanced Graphics for Games](#csc8502---advanced-graphics-for-games)
  - [Controls](#controls)
  - [Building the project](#building-the-project)
    - [Dependencies](#dependencies)
    - [Building on Linux](#building-on-linux)
    - [Building on Windows](#building-on-windows)
  - [Third-Party Assets](#third-party-assets)
  - [Recording the Output](#recording-the-output)
  - [Screenshots](#screenshots)

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
- `O` to skip the camera animation to the end
- `L` to toggle the camera animation
- `N` to skip to the next keyframe in the camera animation

All controls use scancodes, so will be in the same place regardless of keyboard layout.

## Building the project

The project is built using CMake and supports Windows and Linux. MacOS may work, but is completely untested.

### Dependencies

The following libraries must be available on `CMAKE_PREFIX_PATH`:
- [SDL2](https://github.com/libsdl-org/SDL)
- OpenGL (included with Visual Studio on Windows)
- [FPNG](https://github.com/richgel999/fpng) - Included in third-party

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

- `Meshes/quaternius`, `Textures/quaternius`: [Stylized Nature Megakit by Quaternius](https://quaternius.itch.io/stylized-nature-megakit). Used under the [CC0 License](https://creativecommons.org/publicdomain/zero/1.0/) for educational purposes.
- `Textures/crystallotus`: [Skybox Textures by Crystallotus](https://crystallotus.itch.io/skybox-textures). Converted into cubemaps using [Panorama to Cubemap](https://jaxry.github.io/panorama-to-cubemap/).
- `Third Party/SDL2`: [Simple DirectMedia Layer (SDL2)](https://www.libsdl.org/). See
  `Third Party/SDL2/COPYING.txt` for license information.
- `Third Party/fpng`: [FPNG](https://github.com/richgel999/fpng). Used under the [Unlicense](https://unlicense.org/).

## Recording the Output

The `coursework` demo can record to a series of PNG images by passing `-r` as a command line argument. This will save images to the `recording` directory which can be
converted to a video similar to [Source Engine Demos](https://developer.valvesoftware.com/wiki/Demo_Video_Creation) with ffmpeg. The specific command here is:
```sh
ffmpeg -r 60 -start_number 1 -i frame_%05d.png -pix_fmt yuv420p out.mp4
```

Note that this requires ~5GB of disk space for a 30 second, 1080p60 video.

[Demo Video](https://www.youtube.com/watch?v=GKlL0EY-yHE)

## Screenshots

![Present](.resources/panorama.png)
![Future](.resources/panorama_future.png)

The island before and after the apocalypse. The future scene is implemented
as a clone of the past scene, with meshes swapped out and lights added. There
are 500 point lights in the future scene, one for each tree, which are processed
using deferred shading.

Each scene is its own scene graph with its own root node, which allows for
selective rendering of only a single scene or blending between the two.

![Projected Textures and Shadows](.resources/projected_shadow.png)

Projected textures and shadow maps can be applied to any light in the scene.
There is no limit to the number of these as they are all handled in the deferred
pass. Frustum culling is applied to shadow passes to reduce the number of draw
calls.

![Time Warp](.resources/time_warp.png)

The time warp effect is implemented by rendering both scenes to separate
framebuffers then blending and distorting them in a fragment shader. The
distortion is based on sine and triangle waves, which are controlled by the
warp factor.