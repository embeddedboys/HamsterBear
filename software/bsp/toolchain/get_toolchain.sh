#!/bin/bash

function wget_remote()
{
	echo "Downloading  $1"
	wget $1
	if [ $? -eq 0 ]; then
		return 0
	else
		return 1
	fi
}

function do_install_toolchain()
{
	echo "Installing toolchain to $1"
	if [ -d $1 ]; then
		sudo tar xJvf gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabi.tar.xz -C $1
	else
		exit -1
	fi
}

wget_remote https://releases.linaro.org/components/toolchain/binaries/7.5-2019.12/arm-linux-gnueabi/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabi.tar.xz

# echo $?

if [ "$?" -eq "0" ]; then
	do_install_toolchain "/opt"
fi
