#include <cpu/pit.h>
#include <io/io.h>
#include <stdio.h>
#include <mem/heap.h>
#include <mem/paging.h>
#include <mem/pmm.h>

#include <tty/tty.h>
pit_infos_t* pit_infos;
void pit_init(uint32_t freq){
     
     pit_infos = (pit_infos_t*) pmalloc(sizeof(pit_infos_t));
     
     pit_infos->timer_value = 0;
     pit_infos->freq = freq;

     uint32_t div = (7159090 + 6/2) / (6 * freq);

     io_outb(0x43, 0x36);
     io_outb(0x40, (uint8_t)(div & 0xFF));
     io_outb(0x40, (uint8_t)((div>>8) & 0xFF));

}

void pit_sleep(uint64_t t){
     pit_infos->timer_value = t;
     while(pit_infos->timer_value > 0)
          asm volatile ("nop");
}

void pit_event(){
     if(pit_infos->timer_value > 0)
          pit_infos->timer_value--;
}
