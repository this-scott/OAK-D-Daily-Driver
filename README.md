# OakDDaily Driver
Personal project to setup my oakd camera as a daily use webcam instead of using it for it's actual purpose

Feels like a good starter 

Building this out of the [depthai-core-example](https://github.com/luxonis/depthai-core-example)
Build commands are the same so far 
```
cmake -Bbuild -S. -DCMAKE_PREFIX_PATH=/path/to/install/dir //create Build files in ./build, use . as Source, (optional)dependency path
cmake --build build --parallel 4 //build build path, use 4 files at a time
```

Create direct device command
modprobe v4l2loopback video_nr=<N> card_label="Oak-D Camera" width=1920 height=1080 exclusive_caps=1

## Current Program Flow (specify item titles)
1. Setup script
    1. Installs v4l2loopback
    2. Creates and applies a global modprobe rule
    3. Writes a script write will start the loopback device and code
    4. Creates a service that executes the script
    5. Creates a script that starts the service
    6. Attaches this to a udev rule which triggers when the oakd camera is detected

## This may conflict with existing v4l2loopback setups. 
- This program applies a module level setting, exclusive_caps=1 which is common for web programs like Chrome and Discord to recognize it as a webcam device. If your current setup uses v4l2loopback ensure that it doesn't require this parameter set to something else.

Problem: Plugging in the device will start the script, automatically starting the camera. Need to set it up to either start the camera pipeline or start the script when an app (discord, chrome, etc...) calls the camera
**~~Solution: We're moving to pipewire~~**
    *Pipewire only allows us talk to device which read from the pipewire service*

**We're moving to the loopback library**
