# ADA [![Build Status](https://app.travis-ci.com/patriciogonzalezvivo/ada.svg?branch=main)](https://app.travis-ci.com/github/patriciogonzalezvivo/ada)

ADA is an easy cross platform OpenGL ES 2.0 library for creating apps for Windows, MacOS, Linux (with or without an X11 Window Manager) and WASM (Emscripten WebAssembly).

## 1. Install Dependencies

### Windows 

* [Visual Studio 2019 or higher](https://visualstudio.microsoft.com/vs/). Make sure to check "Desktop development with C++" and "Universal Windows Platform development" are installed
* A [git](https://gitforwindows.org/) client 
* CMake (through [Scoop](https://scoop.sh/) is recommended)


### MacOS

```bash
brew install glfw3 pkg-config
```

For video support (using FFMpeg library LIBAV), also do:

```bash
brew install ffmpeg --build-from-source
```

### Linux: Debian based distributions with X11 Window Managers (Ex: Ubuntu, Raspberry Pi OS, etc) 

```bash
sudo apt install git cmake xorg-dev libglu1-mesa-dev
```

For video support (using FFMpeg library LIBAV), also do:

```bash
sudo apt install ffmpeg libavcodec-dev libavcodec-extra libavfilter-dev libavfilter-extra libavdevice-dev libavformat-dev libavutil-dev libswscale-dev libv4l-dev libjpeg-dev libpng-dev libtiff-dev
```

### Linux: Debian based distributions with no X11 Window manager (Raspberry Pi OS)

```bash
sudo apt install git cmake libgbm-dev libdrm-dev libegl1-mesa-dev libgles2-mesa-dev
```

### Linux: Fedora distribution

```bash
sudo dnf install git gcc-c++ cmake mesa-libGLU-devel glfw-devel libXi-devel libXxf86vm-devel 
sudo yum install libXdamage-devel 

```

For video support (using FFMpeg library LIBAV), also do:
```bash
sudo dnf install ffmpeg ffmpeg-devel
```

### Linux: Arch distribution

```bash
sudo pacman -S glu glfw-x11
```

For video support (using FFMpeg library LIBAV), also do:

```bash
sudo pacman -S ffmpeg
```

### Emscripten WebAssembly project

Follow [emscripten installation instructions](https://emscripten.org/docs/getting_started/downloads.html#installation-instructions):

```bash
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
git pull
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
cd ..
```


## 2. Compiling

### For windows managers like MacOS, Windows or Linux (with X11 Window Manager) (all through GLFW) 

```bash
git clone https://github.com/patriciogonzalezvivo/ada_test.git
cd ada_test
mkdir build
cd build
cmake ..
make
./test
```

### For Linux with no X11 Window Manager

```bash
git clone https://github.com/patriciogonzalezvivo/ada_test.git
cd ada_test
mkdir build
cd build
cmake -DNO_X11=TRUE ..
make
./test
```

### As a Emscripten WebAssembly project

```bash
git clone https://github.com/patriciogonzalezvivo/ada_test.git
cd ada_test
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=$EMSDK/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake
make

python3 -m http.server 
```
Then open http://localhost:8000/


# Acknowledgements

This library is highly inspired on [OpenFramewors](https://github.com/openframeworks/openFrameworks), a dear framework and community very close to my heart. The Mesh, Node and Camera clases are VERY similar. The reason to branch ADA was to minimize the amount of code, dependencies and be hable to add it through CMake.

