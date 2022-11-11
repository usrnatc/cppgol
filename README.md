# Conway's Game of Life

This is a simple implementation of Conway's Game of Life in C++ using the
SDL2 library.

See [Releases](https://github.com/usrnatc/cppgol/releases) for Windows build 
and Linux build.

Not too sure how stable the linux build is but I managed to get it to run on:
- Debian testing
- Archlinux

# Prequisites for Linux
- SDL2
- SDL2_ttf
- SDL2_gfx

The Windows binary should run alongside the libSDL2_gfx.dll

![image](https://user-images.githubusercontent.com/79397859/201312754-c0762500-bdc4-4073-8c1d-90e7a1bfc323.png)

# Controls

- `<LeftMouseClick>`  : place alive cell
- `<RightMouseClick>` : kill allive cell
- `<p>`               : pause cells
- `<c>`               : clear screen
