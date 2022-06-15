setenv bootargs console=tty0 console=ttyS1,115200 panic=5 rw rootwait root=/dev/mmcblk0p2 mem=64M
load mmc 0:1 0x80C00000 suniv-f1c100s-hamsterbear-track-wireless.dtb
load mmc 0:1 0x80008000 zImage
bootz 0x80008000 - 0x80C00000
