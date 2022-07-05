# PATCHs dir

This dir stores patchs that need to be apply to bsp project

the `staging` dir stores old patchs that apply like this:
```shell
patch -p1 < xxx.patch
```

the others dir 

- `kernel` - patchs for kernel
- `boot` - patchs for uboot
- `rootfs` - patchs for buildroot

patchs generated like this:
```shell
git format-patch commit-id -o /tmp
```

and need apply like this:

Actully, you don't need do this manually, the build script
will do all this work.

