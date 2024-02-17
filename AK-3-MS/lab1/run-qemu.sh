#!/bin/sh

qemu-system-gnuarmeclipse --verbose --board STM32F4-Discovery --mcu STM32F407VG -d unimp,guest_errors --image firmware.elf --semihosting-config enable=on,target=native -s -S
