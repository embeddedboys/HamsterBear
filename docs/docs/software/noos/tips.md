裸机开发注意事项
---------------

本节介绍F1C200s裸机开发的一些注意事项，例如
- bootrom是什么？
- sram sdram地址空间
- 什么是位置无关码？
- 什么是加载地址，链接地址又是什么？
等

本教程不涉及keil、jlink调试开发，主要基于arm-gcc和Makefile
进行裸机程序的开发。

### 如何加载并运行一个程序？
当你将源程序编译为bin文件之后，这应该是你要考虑的第一个问题，
虽然不同平台有不同的方法，但是本质上就是加载程序到sram中，然后执行之。
这里简单介绍下全志平台的方法：
1. 内存加载运行
```shell
# 加载文件到内存指定位置
sudo sunxi-fel -p write 0x00000000 led.bin
# 执行指定位置的指令
sudo sunxi-fel exec 0x00000000
```
此种方式无法直接访问ddr内存空间，因为没有进行初始化等操作
复位后，cpu重新进入FEL模式

2. flash烧写执行
```shell
# 烧写文件到flash指定位置
sudo sunxi-fel -p spiflash-write 0 led.img
# 复位，让bootROM 执行 led.img
```
led.img 是 修改添加头部信息的 led.bin，过程可参考本文mkimage小节

有些bootROM有初始化ddr的功能，笔者未对flash编程方法进行测试，所以无法确定
是否可以直接访问ddr空间，感兴趣的读者可以自行测试下。

如果需要重新对flash编程，需要短接flash让cpu重新进入FEL模式

注意，两种方式都需要SoC预先处于FEL模式

### 如何进入FEL模式
当BootROM从spi sdio等接口检测不到有效的程序后，cpu会自动进入FEL模式

### BootROM是什么?
BootROM是位于SoC地址空间0x00000000处开始的一小段程序，其固化在SoC内部，
只读，上电后BootROM首先被执行。

细心的读者可能已经发现了，怎么内存加载运行的方式也是加载到0x00000000处运行？
这不是跟BootROM的地址冲突了？有关这个问题，在下面的小节中讨论

### SRAM SDRAM 地址空间
根据芯片手册中的说明：
SRAM A空间为 0x00010000 ~ 0x00019FFF 大小为40KB
DDR1 空间为  0x80000000 ~ 0xBFFFFFFF 大小为1G

F1C100s内置32MB DDR1
F1C200s内置64MB DDR1

所以实际上F1C200s DDR1可寻址的空间为 0x80000000 ~ 0x80400000

对程序进行重定位的时候，需要先初始化ddr，可以参考uboot中dram_suniv.c，
此代码基于xboot中的裸机ddr初始化文件。

有关bootROM地址总线冲突的问题，我个人的猜想如下：
1. SoC在BootROM失效后，其内存控制器将SRAM A的首地址切换为0x00000000
2. SoC内部有一块sram，他将在bootROM失效后，代替BootROM的位置

这里的失效指BootROM找不到可加载的程序

PS：
有条件的同学，可以向全志官方求证这个问题，然后可以修改此文档，答复这个问题。

### 什么是位置无关码？

### 什么是加载地址，链接地址又是什么？

### ARM指令集

### Makefile 示例
```c
PREFIX?=arm-none-eabi-
CC:=$(PREFIX)gcc
LD:=$(PREFIX)ld
AR:=$(PREFIX)ar
OBJCOPY:=$(PREFIX)objcopy
OBJDUMP:=$(PREFIX)objdump

LOAD_ADDR:=0x00000000
LINK_ADDR:=0x80000000

LINK_SCRIPT:=suniv-f1c200s.lds
#LINK_SCRIPT:=suniv.lds

objs:=head.o

default:led.bin

led.bin: clean $(objs)
        $(LD) -T $(LINK_SCRIPT) -g $(objs) -o led.elf

        $(OBJCOPY) -O binary -S led.elf led.bin
        $(OBJDUMP) -D -m arm led.elf > led.dis

led.img: led.bin
        mkimage -C none -A arm -T sunxi_egon -e $(LINK_ADDR) -d led.bin $@

%.o:%.S
        $(CC) -nostdlib -g -c -o $@ $<
%.o:%.c
        $(CC) -nostdlib -g -c -o $@ $<

.PHONY:clean
clean:
        rm -rf *.img *.dis *.elf *.o *.bin

.PHONY:write
write:led.bin
        sudo sunxi-fel -p write $(LOAD_ADDR) $<

.PHONY:write-flash
write-flash:led.img
        sudo sunxi-fel -p spiflash-write 0 $

.PHONY:exec

exec:write
        sudo sunxi-fel exec $(LOAD_ADDR)

.PHONY:ver

ver:
        sudo sunxi-fel ver

```

### link script 示例
```c
MEMORY { .sram : ORIGIN = 0x00010000, LENGTH = 0x000A000 }
MEMORY { .sdram : ORIGIN = 0x80000000, LENGTH = 0x00400000 }

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)
SECTIONS {
        .text      :
        {
                __start = .;
                *(.text)
        } > .sram

        . = ALIGN(4);
        .rodata : { *(.rodata) } > .sram

        . = ALIGN(4);
        .data : {
                *(.data)
         } > .sram

        . = ALIGN(4);
        .bss :
        {
                __bss_start = .;
                *(.bss)
                *(.COMMON)
                __bss_end = .;
        } > .sdram
}

```

### mkimage
当你从spi或者sdio启动时，cpu内部的bootROM会尝试从如上接口读取有效的程序，
如何判断其有效性？
通过如下命令给bin文件添加`sunxi eGon`（魔术头）
```c
mkimage -C none -A arm -T sunxi_egon -e $(LINK_ADDR) -d led.bin $@
```

可以参考uboot源码中`u-boot/tools/mksunxiboot.c`来查看对文件做了什么处理
