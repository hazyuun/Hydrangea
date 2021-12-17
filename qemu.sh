#!/bin/bash

ISO=./yuunos.iso

make iso

case "$1" in
  "--gdb")
    echo "[*] Running qemu in gdb debug mode"
    qemu-system-i386 -m 4G -cdrom $ISO -serial stdio -no-reboot -no-shutdown -s -S
  ;;

  *)
    qemu-system-i386 -m 4G -cdrom $ISO -serial stdio -no-reboot -no-shutdown
  ;;

esac
