Build Modules
=============
  make ARCH=arm ea6400_defconfig
  make ARCH=arm CROSS_COMPILE=/opt/toolchains/arm_cortex-a9_gcc-8.2.0_musl_eabi/bin/arm-linux- modules

find -name *.ko
