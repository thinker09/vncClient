# vncClient
a simple exaple of vnc client based libvncserver
# VNC Client Project

This project is a VNC client built using the `libvncserver` library, specifically version 0.9.15. It has been developed and tested on Ubuntu 20.04, utilizing Qt version 5.14.2. The project is based on the example files provided with `libvncserver`, which have been modified and enhanced to create the current project.

## Features

- **Smooth VNC view and responsive mouse and keyboard operations**: The connection with server is very smooth.
- **Keyboard Input Support**: The client now supports keyboard input, allowing users to interact with the remote VNC server using their keyboard.
- **Case Sensitivity**: The client distinguishes between uppercase and lowercase letters(with shift Key), ensuring accurate input handling.

## Optimizations

1. **Keyboard Input**: Enhanced keyboard input handling to support a broader range of keys(set up key mapping between qt key and rfbKeySym).
2. **Case Sensitivity**: Implemented case sensitivity to differentiate between uppercase and lowercase characters.

## Work in Progress

- **Special Key Mappings**: Some special keys are not yet fully mapped or supported. This includes keys like function keys, arrow keys, and modifier keys.

## Getting Started

### Rerequisites

- **Ubuntu 20.04**: This project is developed and tested on Ubuntu 20.04.
- **Qt 5.14.2**: The project uses Qt version 5.14.2 for the graphical user interface.
- **libvncserver 0.9.15**: The project is based on `libvncserver` version 0.9.15.

### Installation

1. **Install Dependencies**:

   ```
   install qt with online installer.
   ```

2. **Steps to Build and Set Up LibVNCserver**
&emsp;2.1 Clone the Repository (Using the latest version: 0.9.15)

``` bash
    git clone https://github.com/LibVNC/libvncserver
```

&emsp;&emsp;&emsp;2.2 Build the Code
&emsp;&emsp;&emsp;Navigate to the project's root directory and follow these steps:

``` bash
    mkdir build
    cd build
    cmake .. -DWITH_OPENSSL=ON -DWITH_GCRYPT=OFF
    cmake --build .
```

&emsp;&emsp;&emsp;2.3 Set Up the libvnc Directory
&emsp;&emsp;&emsp;Create a new directory named libvnc, and inside it, create two subdirectories: lib and rfb.
&emsp;&emsp;&emsp;Copy the following files:
```
    From libvncserver/build/include/rfb, copy rfbclient.h to libvnc/include.
    From libvncserver/include, copy all header files to libvnc/include.
    From libvncserver/build, copy the following shared libraries to libvnc/lib:
    libvncclient.so
    libvncclient.so.0.9.15
    libvncclient.so.1
```
