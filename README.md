# ADA [![Build Status](https://travis-ci.org/patriciogonzalezvivo/ada.svg?branch=master)](https://travis-ci.org/patriciogonzalezvivo/ada)


The fundation of any visual computational adventure. 
Ada is an easy cross platform OpenGL ES 2.0 library for creating apps in Windows, MacOS and Linux (with or without X11 support like in RaspberrPi)

## Dependencies


### Ubuntu 

```
sudo apt install git cmake xorg-dev libglu1-mesa-dev
```

### RaspberryPi OS (with X11)

```
sudo apt install git cmake xorg-dev libglu1-mesa-dev
```

### RaspberryPi OS (without X11)

```
sudo apt install git cmake libegl1-mesa-dev libgbm-dev libgles2-mesa-dev
```

Make sure on your `/boot/config.txt` you have the following lines present and uncommented:

```
dtoverlay=vc4-fkms-v3d
max_framebuffers=2
hdmi_force_hotplug=1
```

### Fedora

```
sudo dnf install git cmake mesa-libGLU-devel glfw-devel libXi-devel
```

### Arch 

```
sudo pacman -S glu
```

## Compiling Ada

### For windows managers like Windows, MacOS, Linux wm like Gnome/KDE/etc (all through GLFW) 

```bash
git clone https://github.com/patriciogonzalezvivo/ada.git
cd ada
mkdir build
cd build
cmake ..
make
sudo make install
```

### Compiling ADA for no X11 context (GBM by default or BROADCOM drivers on Raspberry PI)

```bash
git clone https://github.com/patriciogonzalezvivo/ada.git
cd ada
mkdir build
cd build
cmake -DNO_X11=TRUE ..
make
sudo make install
```

**Note**: Newer RaspberryPi distribution do have BROADCOM drivers but work only on GBM for that use `-DFORCE_GBM=TRUE` instead

## Building your own app with ADA


Add the ADA header files you need

```c++
#include "ada/window.h"
#include "ada/gl/gl.h"

...

int main(int argc, char **argv) {
    // Initialize openGL context
    ada::initGL(argc, argv);

     // Render Loop
    while ( ada::isGL() ) {
        // Update
        ada::updateGL();

        // Do your thing
        // ...

        // Render the scene
        ada::renderGL();

        // Update the viewport
        ada::updateViewport();
    }

    // Close the GL context
    ada::closeGL();

    return 1;
}

```

Then on the `CMakeList.txt` you just need to do:

```
add_executable (myApp myApp.cpp)

set_target_properties(myApp PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)

target_link_libraries (myApp PRIVATE ada)
```


