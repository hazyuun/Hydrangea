#!/bin/bash

ISO=./yuunos.iso

make iso

case "$1" in
  "--gdb")
    echo "[*] Running qemu in gdb debug mode"
    qemu-system-i386 -cdrom $ISO -serial stdio -s -S
  ;;

  *)
    qemu-system-i386 -cdrom $ISO -serial stdio -no-reboot -no-shutdown
  ;;

esac
