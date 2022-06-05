<!--
 Copyright (c) 2022 IotaHydrae
 
 This software is released under the MIT License.
 https://opensource.org/licenses/MIT
-->

整个系统的启动流程大致如下

bootROM --> uboot-spl --> uboot -> kernel

bootROM 是固化在SoC内部的一小段引导程序，用来检测SDC SPI等接口上有无可加载引导的固件，其引导流程大概如下：

![Boot Sequence](../assets/boot_sequence.png "")

上图为全志A10的引导流程，同样适用于F1C100s