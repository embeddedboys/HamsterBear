本项目linux内核使用主线5.17.0版本
可通过kernel.org直接下载压缩包
所做修改位于同级目录下kernel_patch文件
拷贝hamsterbear_kernel_defconfig至arch/arm/configs/下
执行ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- make hamsterbear_kernel_defconfig
ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- make zImage -j12

## 源码获取


