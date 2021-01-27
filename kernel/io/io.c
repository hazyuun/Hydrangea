/*
 *	File : io.c
 *	Description : TODO
 * */

#include <io/io.h>

void io_outb(uint16_t port, uint8_t data) {
  asm volatile("outb %0, %1" : : "a"(data), "Nd"(port));
}

uint8_t io_inb(uint16_t port) {
  uint8_t result;
  asm volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
  return result;
}

void io_outl(uint16_t port, uint32_t data) {
  asm volatile("outl %0, %1" : : "a"(data), "d"(port));
}

uint32_t io_inl(uint16_t port) {
  uint32_t result;
  asm volatile("inl %1, %0" : "=r"(result) : "d"(port));
  return result;
}

uint16_t io_inw(uint16_t port) {
  uint16_t result;
  asm volatile("inw %1, %0" : "=a"(result) : "dN"(port));
  return result;
}

void io_outw(uint16_t port, uint16_t data) {
  asm volatile("outw %1, %0" : : "dN"(port), "a"(data));
}