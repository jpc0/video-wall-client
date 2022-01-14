#!/bin/sh

# export RASPBIAN_ROOTFS=/opt/sysroot/
# export PATH=/opt/cross-pi-gcc/bin:$PATH
# export LD_LIBRARY_PATH=/opt/cross-pi-gcc/lib:$LD_LIBRARY_PATH
# export RASPBERRY_VERSION=3
mkdir rpi-build
cd rpi-build/ && cmake -DCMAKE_TOOLCHAIN_FILE=../Toolchain-rpi.cmake ..