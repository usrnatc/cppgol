### Conway's Game of Life

This is a simple implementation of Conway's Game of Life in C++ using the
SDL2 library.

See [Releases](https://github.com/usrnatc/cppgol/releases) for Windows build 
and Linux build.

### Installation
## Linux
# Debian-based:
```bash
sudo apt install libsdl2 libsdl2-ttf libsdl2-gfx
```
Find a prebuilt binary from the [Releases Page](https://github.com/usrnatc/cppgol/releases)

## Windows x64:
Find a prebuilt binary from the [Releases Page](https://github.com/usrnatc/cppgol/releases)

Extract `cppgol_win_x86-64.7z`, and run `cgol.exe`

- If you have any problems running on Windows, it is most likely due to your
antivirus software as this is not a signed binary

![image](https://user-images.githubusercontent.com/79397859/201312754-c0762500-bdc4-4073-8c1d-90e7a1bfc323.png)

# Controls

- p                     : Pause simulation
- c                     : Clear current board
- Scroll Up             : Increase simulation tickrate
- Scroll Down           : Decrease simulation tickrate
- Left Mouse            : Place cell
- Right Mouse           : Remove cell
- Left/Right Arrow Keys : change current brush
