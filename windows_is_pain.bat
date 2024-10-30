@REM CMake on windows is a necessary evil, but it's still evil
@REM this script sets up the environment for CMake to work

@REM This only works on my specific setup, but windows is suffering
SET CMAKE_PREFIX_PATH=H:\cmake

cd build
cmake ..