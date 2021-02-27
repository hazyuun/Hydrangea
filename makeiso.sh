#!/bin/bash

make kernel
if [ $? -ne 0 ]
then
  exit
fi

cp bin/yuunos.bin iso/boot
rm iso/yuunos.iso
grub-mkrescue -o iso/yuunos.iso iso
cp iso/yuunos.iso .




