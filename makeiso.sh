#!/bin/bash
make ker && cp bin/yuunos.bin iso/boot && rm iso/yuunos.iso && grub-mkrescue -o iso/yuunos.iso iso && cp iso/yuunos.iso .



