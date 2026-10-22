# OAK-D Daily Driver
Driver for using an [OAK-D computer vision camera](https://shop.luxonis.com/collections/oak-cameras-col) as a traditional webcam

This functionality existed with Depthai-core V2 but was disgarded in V3

The stream currently outputs a colored 1080p stream at 30fps. Looking into options for the picture to be configured

Tested on an OAK-D S2

## Features
- Regular webcam interface which can be used with any program (Browser, Zoom, etc.)
- Udev rules and listeners start/stop the camera when requested so no resources are wasted
- Lightweight Debian package install dependencies automatically for easy setup and removal

## How it works
- Udev rule listens for the vendor signature OAK-D cameras provide. Activates service on trigger
    - The signature belongs to Intel Movidius, the VPU built into OAK-D cameras. Will activate service on Intel NCS2 Sticks
- Service runs the program which creates then configures a v4l2loopback device and client-usage event listener
- The listener triggers when clients watching changes. If `client-usage > 0`, the camera activates, streaming to the loopback device. When 0, the camera cleans up and the activate the listener again
- On sigterm or sudden exit the program cleans up all created objects and closes safely

## Installation
### Debian
- **Requires Ubuntu 25 or above**
- A debian package is avaliable for download in releases
- Client-usage event was introduced in `v4l2loopback >= 13.0.0` forcing the Ubuntu 25 requirement (tested on ubuntu 26)
- The package comes with a lightweight (opencv other options were disabled during build) version of the depthai-core driver compiled


### Build from Source and configure activation rules
```bash
# Configure build
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/path/to/depthai-install

# Build and install, service uses /usr as space for install applications
cmake --build build --parallel 4
cmake --install build --prefix /usr

# Activate udev rule and service by reloading both
devadm control --reload-rules
systemctl daemon-reload
```
**Requires**
- [depthai-core](https://github.com/luxonis/depthai-core/tree/main)
- v4l2loopback version >= 13.0.0 `apt install v4l2loopback-dkms`
- cmake `apt install cmake`
