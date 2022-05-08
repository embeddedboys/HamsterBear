<!--
 Copyright (c) 2022 Zheng hua
 
 This software is released under the MIT License.
 https://opensource.org/licenses/MIT
-->

# **软件文档**

## 说明
这些文档中总结出的资料和经验，有相当一部分都是从网络上学习得来的，这里需要给出参考过的文章链接，比较重要的放在这里，其余的放在本文末尾。

https://linux-sunxi.org 这是全志linux最大的社区了，里面记录了全志SoC Family的各种资料

https://whycan.com 挖坑网论坛，主要玩全志芯片的，也有其他，全志为主，里面大佬云集，不会的可以发帖问他们（狗头），我也会经常关注

## 系统移植篇

### [全志suniv启动流程](suniv-boot.md)

### [U-Boot](uboot.md)

### [Linux](Linux.md)

### [Linux Modules](modules.md)

### [Buildroot](buildroot.md)
### [Debian](debian.md)

### [最小系统镜像打包](post-image.md)

### [镜像的烧录](burn-image.md)

## 驱动开发篇

### GPIO

### I2C

### SPI

### PWM

### LCD

### IR

### ADC

### RTC

### SDIO

### DMA

### UART

###  CSI

### TP

### Audio Codec

## 软件开发篇

### LVGL标准移植

### Joystick输入

### LVGL应用开发

### 用户空间的I2C调用

### 用户空间的SPI调用

### 用户空间的DMA调用

### 用户空间的uinput模拟输入事件

### 串口编程

## 裸机开发

### [裸机开发的一些注意事项](noos/tips.md)

### [blink](noos/blink.md)

### 重定位

### 时钟初始化

### DDR初始化

### 裸机UART

### 裸机I2C

### 裸机SPI

## RTOS移植