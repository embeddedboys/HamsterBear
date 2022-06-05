<!--
 Copyright (c) 2022 IotaHydrae
 
 This software is released under the MIT License.
 https://opensource.org/licenses/MIT
-->

全志 suniv SoC 家族有如下型号：

| Allwinner F23 aka F1C100A | (suniv) |
| ------------------------- | ------- |
| Allwinner F25 	        | (suniv) |
| Allwinner F1C100A 	    | (suniv) |
| Allwinner F1C100s	        | (suniv) |
| Allwinner F1C200s	        | (suniv) |
| Allwinner F1C500 	        | (suniv) |
| Allwinner F1C500s 	    | (suniv) |
| Allwinner F1C600 	        | (suniv) |
| Allwinner F1C800 	        | (suniv) |
| Allwinner F1D100 	        | (suniv) |

本项目主要涉及[F1C100s](https://www.allwinnertech.com/index.php?c=product&a=index&id=73)和[F1C200s](https://www.allwinnertech.com/index.php?c=product&a=index&id=74)，[F1C200s](https://www.allwinnertech.com/index.php?c=product&a=index&id=74)为100s的增强版本

[F1C100s](https://www.allwinnertech.com/index.php?c=product&a=index&id=73) 硬件参数

| Manufacturer	| Allwinner |
| ------------- | --------- |
| CPU	        | ARM ARM926EJ-S @ 533MHz |
| Memory	    | 32MB Embedded DDR1 |
| GPU	        | Custom 2D-only |
| VPU	        | Unknown |
| Connectivity  |
| Video	        | LCD, CVBS |
| Audio	        | DAC |
| Storage	    | SD/MMC (SD v2.0, | eMMC V4.41) |
| USB	        | USB 2.0 OTG |
| Website	    | [Product Page](https://www.allwinnertech.com/index.php?c=product&a=index&id=73) |

[F1C200s](https://www.allwinnertech.com/index.php?c=product&a=index&id=74) 在硬件参数上与 [F1C100s](https://www.allwinnertech.com/index.php?c=product&a=index&id=73) 大体保持一致，区别如下:

`F1C100s`
* `Memory`- 32MB Embedded DDR1
* `Video` - H.264 1280x720@30fps decoding
            MPEG1/2/4 1280x720@30fps decoding
            MJPEG 1280x720@30fps encoding
            JPEG encode size up to 8192x8192

`F1C200s`
* `Memory`- 64MB Embedded DDR1
* `Video` - H.264 1920x1080@30fps decoding
            MPEG1/2/4 1920x1080@30fps decoding
            MJPEG 1280x720@30fps encoding
            JPEG encode size up to 8192x8192


（参考自 [https://linux-sunxi.org](https://linux-sunxi.org)）