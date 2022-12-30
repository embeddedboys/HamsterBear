HamsterBear
-----------

[[中文](README.md)] [[English](README.en.md)]

![HamsterBear](docs/docs/assets/hamsterbear_real.jpg "HamsterBear")

HamsterBear is a Embedded linux develop board based on Allwinner Tech F1C200s SoC

The website of this project:
[`https://embeddedboys.github.io/HamsterBear/`](https://embeddedboys.github.io/HamsterBear/)

Hardware
-------

### Core Board

* `SoC` : F1C200s arm926ejs

* `DDR` : 64MB on chip

* `Disk` : Nand Flash or TF card

* `USB OTG` : MiniUSB

* `GPIO Expand` : TCA9536

* `FPC Interface` : CSI TFT

* `Header` : PE0-11 PA0-3

* `M2 NGFF` : 67Pins M-KEY 

### Base Board

* `USB OTG or USB HUB` : The F1C200s only have one group of USB signal, so need a jumper to determine OTG or HUB is used

  * `USB OTG` : a MicroUSB interface

  * `USB HUB` : FE1.1s with eeprom

* `Wireless` : RTL8188EUS (only can be used when HUB is enabled)

* `3 USB` : USB Type-A interface (only can be used when HUB is enabled)

* `Display` : 1.69 inch TFT ST7789V 280x240

* `RTC` : DS1307

* `Audio` : PJ-327A

* `Keys` : ADC key tablet

* `GPIO Expand` : PCA9536

* `2 UART` : Header

* `2 I2C` : Header

* `1 SDIO` : Header

* `1 SPI` : Header

Software
-------

* `U-Boot` : 
  * a bsp uboot can boot from SDcard
  * a mainline uboot boot from nand flash 

* `Kernel` : mainline v5.17.0

* `Buildroot` : mainline v2022.2

    * directfb
    * X server
    * Qt5
    * GTK
    * Python3.10
    * etc

The network connection can be used like:
1. USB Network Share (g_ether)


2. Wireless RTL8188EUS on base board

More
----

For more helpful information, check offical website and README files.
