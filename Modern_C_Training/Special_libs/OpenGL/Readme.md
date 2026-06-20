## DETAILS

- PROJECT: MODERN OPENGL 4.6 CORE PROFILE WITH C23
- Target Stack: Debian 13 (Trixie), GCC 15, CMake, KDE Plasma (Wayland)
--------------------------------------------------------------------------------

1. SYSTEM PREREQUISITES (DEBIAN 13)

Open your terminal in KDE Plasma and install the required compiler tools, 
CMake, OpenGL drivers, and the GLFW 3.4 library:

```
$ sudo apt update
$ sudo apt install build-essential cmake xorg-dev libgl1-mesa-dev libglfw3-dev mesa-utils
```

Verify that your GPU and drivers support OpenGL 4.6 Core Profile:
```
$ glxinfo | grep -E "OpenGL core profile version string"
```
--------------------------------------------------------------------------------
2. MANUAL GLAD GENERATION & DIRECTORY STRUCTURE

GLAD manages modern OpenGL function pointers dynamically. You must generate 
the headers matching your system version manually:

1. Visit the web generator: https://glad.dav1d.de/
2. Configure the exact options:
   - Language: C/C++
   - Specification: OpenGL
   - API gl: Version 4.6
   - Profile: Core
   - Options: Ensure "Generate a loader" is turned ON.
3. Click "Generate" and download "glad.zip".
4. Extract the contents into your project root folder matching this structure:

```
project/
├── CMakeLists.txt
└── src/
    ├── external/
    │    └── glad/
    │        ├── include/
    │        │   ├── glad/
    │        │   │   └── glad.h
    │        │   └── KHR/
    │        │       └── khrplatform.h
    │        └── src/
    │            └── glad.c
    └── main.c
```

--------------------------------------------------------------------------------
3. COMPILATION AND EXECUTION STEPS

From your project's directory, run the following commands in the shell:

```
$ cmake -B build
$ cmake --build build/
$ ./build/helloWorld
```
