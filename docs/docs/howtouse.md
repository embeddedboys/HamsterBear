(这里放一张接口定义图)

# 上手教程

## 介绍

`HamsterBear` 主板一块

`USB转串口模块` 一个

`MiniUSB线`一根

## 串口调试
默认的设备树中 `UART0` 复用到如下管脚：
* `PE0` : UART0_RX 
* `PE1` : UART0_TX

如果连接了底板，还可使用UART1、UART2，复用如下：
* `PD3` : UART1_RX
* `PD4` : UART1_TX

* `PD14` : UART2_RX
* `PD13` : UART2_TX

## USB调试

请先按照该链接中的说明安装驱动程序：
(USB以太网驱动安装教程链接)

连接好usb线后，待系统启动完成，PC端会发现新的网络设备（未完）
