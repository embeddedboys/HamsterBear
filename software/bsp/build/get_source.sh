#!/bin/bash

RED_COLOR='\E[1;31m'  #红
GREEN_COLOR='\E[1;32m' #绿
YELOW_COLOR='\E[1;33m' #黄
BLUE_COLOR='\E[1;34m'  #蓝
PINK='\E[1;35m'      #粉红
RES='\E[0m'

UBOOT_RESOURCE=u-boot-master.tar.gz
KERNEL_RESOURCE=linux-5.17.2.tar.xz
BUILDROOT_RESOURCE=buildroot-2022.02.tar.gz

UBOOT_RESOURCE_URL=https://source.denx.de/u-boot/u-boot/-/archive/master/${UBOOT_RESOURCE}
KERNEL_RESOURCE_URL=https://cdn.kernel.org/pub/linux/kernel/v5.x/${KERNEL_RESOURCE}
BUILDROOT_RESOURCE_URL=https://buildroot.org/downloads/${BUILDROOT_RESOURCE}

DOWNLOAD_TOOL=wget

DEST_DIR=/tmp

function echo_message()
{
	echo -e "${GREEN_COLOR} MESSAGE: ${RES} $1"
}

function usage()
{
	echo -e "Usage: $0 -<option> <parameter>"
	echo -e "\toptions:"
	echo -e "\t\t -d"
	echo -e "\tparameterss:"
	echo -e "\t\t uboot"
	echo -e "\t\t kernel"
	echo -e "\t\t buildroot\n"
	echo -e "example: $0 -get=kernel\n"
}

function do_get_uboot()
{
	echo_message "Downloading uboot resource"
	
	if [ -f "/tmp/${UBOOT_RESOURCE}" ]; then
		echo_message "File exists ${UBOOT_RESOURCE}"
		return 0;
	fi

	${DOWNLOAD_TOOL} ${UBOOT_RESOURCE_URL} -O /tmp/${UBOOT_RESOURCE}
	if [ $? -ne 0 ]; then
		echo_message "Failed to download..."
		return 1;
	fi
	return 0;
}

function do_get_kernel()
{

	echo_message "Downloading kernel resource"

	${DOWNLOAD_TOOL} ${KERNEL_RESOURCE_URL} -O /tmp/${KERNEL_RESOURCE}
	if [ $? -ne 0 ]; then
		echo_message "Failed to download..."
		return 1;
	fi
	return 0;
}

function do_get_buildroot()
{
	echo_message "Downloading buildroot resource"
	
	${DOWNLOAD_TOOL} ${BUILDROOT_RESOURCE_URL} -O /tmp/${BUILDROOT_RESOURCE}
	if [ $? -ne 0 ]; then
		echo_message "Failed to download..."
		return 1;
	fi
	return 0;
}

function do_get_all()
{
	do_get_uboot
	do_get_kernel
	do_get_buildroot
	
	return 0;
}

echo $#

if [ $# -eq 0 ]; then
	usage
	echo_message "Seems you don't give a choice"
	echo_message "Do you want to download all resources?[y/N]"
	read choice
	if [ $choice = "y" -o $choice = "Y" ]; then
		echo_message "Downloading all resource..."
		do_get_all
	else
		echo_message "abort"
	fi
else
	echo_message "Downloading all your choice"
	parameters=`getopt -o d: -n "$0" -- "$@"`
	echo "$parameters"
	eval set -- "$parameters"
	echo $1 $2
	while true ; do
		case "$1" in
			"-d")
				case "$2" in
					"all") do_get_all;shift 2;;
					"uboot") do_get_uboot;shift 2;;
					"kernel") do_get_kernel;shift 2;;
					"buildroot") do_get_buildroot;shift 2;;

					*) echo_message "no such option";break;;
				esac;;
			"--")break;;
		esac
	done
fi


