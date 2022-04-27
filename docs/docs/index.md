
# ![Hamster](assets/hamster_64.png) 仓鼠熊 HamsterBear
------

官方网站:
[`https://embeddedboys.github.io/HamsterBear`](https://embeddedboys.github.io/HamsterBear)

仓库地址:
[`https://github.com/embeddedboys/HamsterBear`](https://github.com/embeddedboys/HamsterBear)

![HamsterBear](assets/hamsterbear_real.jpg "HamsterBear")

[**`HamsterBear(仓鼠熊)`**]() 是基于[`Allwinner（全志科技）`](https://www.allwinnertech.com/)[`F1C200s`]() SoC 设计的嵌入式 Linux 开发板，我们在 `3x5cm` 尺寸上的PCB集成了众多功能，并且采用了M.2接口，方便开发多种多样的底板。

为了让其更加生动形象，我们给`HamsterBear`的底板取了个外号叫做**`履带(Track)`**，这是我们给它设计的第一款底板 **`HamsterBear Track Wireless`**
![HamsterBear Track Wireless](assets/hamsterbear_track_wireless_real.jpg "HamsterBear Track Wireless")

![HamsterBear Track Wireless](assets/hamsterbear_track_wireless_real_top.jpg "HamsterBear Track Wireless")


## 软件配置
------

* `SoC` : F1C200s arm926-ejs


* `Kernel` - mainline v5.17


* `U-Boot` - mainline v2022.2


* `Buildroot` - v2022.2
    X Server
    Gtk
    python3
    VNC
    Qt5等


## 工程结构
------
    docs/                           # 本站工程
        
    hardware/                       # 有关硬件设计的部分
        core/                       # 核心板
        datasheets/                 # 芯片手册
        tracks/                     # 底板

    software/                       # 有关软件设计的部分
        bootable/                   # 可启动文件
        bsp/                        # 项目的最小系统工程
            u-boot/                 # u-boot工程
            linux/                  # linux工程
            buildroot/              # buildroot工程
            toolchain/              # 交叉编译工具链

        modules/                    # 外部的驱动程序
        packages/                   # 外部的应用软件
        scripts/                    # 脚本

    references/                    # 本项目所参考的内容

    LICENSE                        # 本项目的许可证
    AUTHORS                        # 本项目涉及到的作者
    THANKS                         # 致谢
    README.md                      # 项目自述文件

## 说明
------
如果你打算从该工程的基础上进行二次开发，请阅读开发者页面中的内容。

工程发布于 MIT 许可证下，有关该许可证的详细信息，请到 `关于 -> 许可证` 中查看

## FAQ
------

#### 1. 酷，我接下来应该做什么？
**`用户向导 -> 快速上手`** 章节里介绍了基本的用法，更详细的用法位于 **`使用文档`** 中

#### 2. 网站里文档资料好少

资料还在整理更新中，正在爆肝中！ 相关文档会先在我的博客园[hfwz](https://cnblogs.com/hfwz)发布，
然后会尽快同步到这里。

#### 3. 项目都有哪些资料开源了呢？

软硬件全部开源，核心板是AD工程，底板LCEDA专业版，工程以及gaber文件都有，软件bsp+应用+驱动全套，
这部分资料和文档还在整理编写中，uboot、kernel、buildroot全部都用的主线，工程内有下载脚本，
下载下来打上提供的补丁就可以直接编译。

#### 4. 为什么都要用主线代码？

个人认为，不玩主线，等于白玩！在主线代码上进行开发，即使不至于向社区提交补丁，但你也可以参与到
内核的开发当中来！当然也得有一定基础！

主线代码还有很多坑没填好，这些坑是不是就等着优秀的你就来填！