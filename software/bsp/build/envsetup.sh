# exporting kernel build target
export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabi-
export PATH=$PATH:/opt/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabi/bin/

# export dirs
export ROOT_DIR=/home/$(whoami)/source/HamsterBear/software/bsp/
echo $ROOT_DIR
