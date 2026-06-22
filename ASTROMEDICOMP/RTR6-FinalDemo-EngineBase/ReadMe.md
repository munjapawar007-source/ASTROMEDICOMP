# SWEET - RTR6

Minimal Win32 OpenGL engine in C++ using GLEW.
Provides window setup, rendering loop, and a basic engine structure (camera, mesh, shader, scene object).

## Build

* Requires Windows, Visual Studio, OpenGL, GLEW
* Link: `opengl32.lib`, `glew32.lib`

## Run

* Execute `bin/SWEET.exe` or use `run.bat`

## Controls

* ESC: Exit
* F: Toggle fullscreen

## Structure

* `src/` – engine source files
* `engineResources/` – shaders and resources
* `external_include/` – math library
* `bin/` – executable
* `intermediate/` – build files

## Notes

Basic rendering with logging to `Log.txt`.
