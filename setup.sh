#!/bin/bash
set -euo pipefail


APP_NAME="Oak-D-Webcam"
BRIDGE_BIN="Oak-D-Bridge"
PREFIX="/usr/local/"
BUILD_DIR="build"

echo "==> Checking v4l2loopback"
if 
