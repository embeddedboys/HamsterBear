#!/bin/bash

# Hi, guys
# 
# This is the main build script of this bsp project and
# the best way to build kernel and etc. is run this script.
#
# Seems we need those functions:
# 
# - build bootloader
#
# - build kernel
#
# - build rootfs
#
# - make a flash or sdcard image
#
# - depoly to device
#
#
# In the build bootloader function or etc, we need do those works:
#
# - (get source from server)
# 1. source the env file, export the toolchain to PATH
# 2. decompress the source tar package
# 3. apply the patch file to current build target
# 4. copy defconfig file under target dir
# 5. make target defconfig
# 6. use output image file
#
# Let's make it
# 
# Cheers,
# IotaHydrae

# TODO: add utils for build function
function echo_message()
{
	echo $1
	return 0
}


# TODO: add bootloader build function
function build_bootloader()
{
	return 0
}

# TODO: add build kernel function
function build_kernel()
{
	return 0
}

# TODO: add build rootfs function
function build_rootfs
{
	return 0
}
# script main
case "$1" in
	boot)
		echo_message "build bootloader"
		;;
	kernel)
		echo_message "build kernel"
		;;
	rootfs)
		echo_message "build rootfs"
		;;
	all)
		echo_message "build all"
		;;
	*)
		echo_message "hello"
		;;

esac
