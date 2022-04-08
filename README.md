HamsterBear
-----------

![HamsterBear](docs/docs/assets/hamsterbear_real.jpg "HamsterBear")

HamsterBear 是基于全志 F1C200s 设计的嵌入式Linux开发板

访问本项目的官网网站（建设中）：
[`https://embeddedboys.github.io/HamsterBear/`](https://embeddedboys.github.io/HamsterBear/)

硬件配置
-------

### 核心板

* `SoC` : F1C200s arm926ejs

* `DDR` : 64MB on chip

* `Disk` : Nand Flash or TF card

* `USB OTG` : MiniUSB

* `GPIO Expand` : TCA9536

* `FPC Interface` : CSI TFT

* `Header` : PE0~11 PA0~3

* `M2 NGFF` : 67Pins M-KEY 

### 底板

* `USB OTG 或 USB HUB` : 由于SoC只有一组USB，所以需要跳线控制连接到OTG或HUB

* `USB OTG` : MicroUSB

* `USB HUB` : FE1.1s with eeprom

* `Wireless` : RTL8188EUS （仅限HUB可用时）

* `3 USB`

* `Display` : 1.69 inch TFT ST7789V

* `RTC` : DS1307

* `Audio` : PJ-327A

* `Keys` : ADC

* `GPIO Expand` : PCA9536

* `2 UART` : UART1 UART2

* `2 I2C` : I2C1 I2C2

* `1 SDIO` : SDC1

* `1 SPI` : SPI0

软件配置
-------

* `U-Boot` : 

* `Kernel` : v5.17.0

* `Buildroot` : v2022.2

    * directfb
    * X server
    * x11vnc
    * Qt5 (默认配置未添加)
    * GTK
    * Python3.10

网络连接可通过如下方式：
1. USB OTG网络共享（g_ether）
2. 底板RTL8188EUS无线模组

更多说明见仓库内各目录下自述文件
