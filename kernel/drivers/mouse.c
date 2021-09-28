#include <drivers/mouse.h>
#include <drivers/ps2.h>
#include <cpu/registers.h>
#include <cpu/pic.h>
#include <cpu/irq.h>
#include <io/io.h>
#include <stdio.h>
#include <term/term.h>

uint8_t mouse_current_byte = 0;

int32_t x = 0;
int32_t y = 0;
int32_t vx = 0;
int32_t vy = 0;
uint32_t xs = 1;
uint32_t ys = 1;

/* 
  Note : The mouse cursor is just for testing purposes,
  when time comes for the window manager, cursor and stuff
  will be part of it actually and not here in the kernel
*/

#define W 0xFFFFFF // W = White
#define B 0x000000 // B = Black
#define N 0x000000 // N = None (it's just black actually)

uint32_t cursor[20][12] = {
  {B, N, N, N, N, N, N, N, N, N, N, N},
  {B, B, N, N, N, N, N, N, N, N, N, N},
  {B, W, B, N, N, N, N, N, N, N, N, N},
  {B, W, W, B, N, N, N, N, N, N, N, N},
  {B, W, W, W, B, N, N, N, N, N, N, N},
  {B, W, W, W, W, B, N, N, N, N, N, N},
  {B, W, W, W, W, W, B, N, N, N, N, N},
  {B, W, W, W, W, W, W, B, N, N, N, N},
  {B, W, W, W, W, W, W, W, B, N, N, N},
  {B, W, W, W, W, W, W, W, W, B, N, N},
  {B, W, W, W, W, W, W, W, W, W, B, N},
  {B, W, W, W, W, W, W, B, B, B, B, B},
  {B, W, W, W, W, W, W, B, N, N, N, N},
  {B, W, W, W, W, W, W, B, N, N, N, N},
  {B, W, W, B, B, W, W, B, N, N, N, N},
  {B, W, B, N, N, B, W, W, B, N, N, N},
  {B, B, N, N, N, B, W, W, B, N, N, N},
  {B, N, N, N, N, N, B, W, W, B, N, N},
  {N, N, N, N, N, N, B, W, W, B, N, N},
  {N, N, N, N, N, N, N, B, B, N, N, N},
  
  };

void vesa_swap_buffers();

void mouse_event(registers_t *r) {
  (void) r;
  uint8_t p = io_inb(0x60);
  
  if(mouse_current_byte == 0)
  {
    if(p & MOUSE_PACKET_X_OVERFLOW || p & MOUSE_PACKET_Y_OVERFLOW)
      return;
    xs = (p & MOUSE_PACKET_X_SIGN);
    ys = (p & MOUSE_PACKET_Y_SIGN);
    ++mouse_current_byte;
    return;
  }
  if(mouse_current_byte == 1){
    vx = xs ? (int8_t) p: p;
    ++mouse_current_byte;
    return;
  }
    
  vy = ys ? (int8_t) p: p;

  x += vx;
  y -= vy;
  x = (x > 1024)? 1024: x;
  y = (y > 768)? 768: y;
  x = (x < 0)? 0: x;
  y = (y < 0)? 0: y;
  
#if 0
  // term_clear();
  vesa_back_buffer_clear();
  // printk("\n\n\n\t\t\t\t Oooh shit ! A mouse !");
  for(uint8_t i = 0; i < 12; i++){
    for(uint8_t j = 0; j < 20; j++){
      vesa_back_buffer_put_pixel(x+i, y+j, cursor[j][i]);
    }
  }
  vesa_swap_buffers();
#endif
  mouse_current_byte = 0;
}

#include <util/logger.h>

void mouse_init(uint8_t ch){
  ps2_ctrlr_cmd(PS2_CMD_READ_CFG);

  uint8_t cfg = ps2_inb(PS2_DATA);

  cfg |= ch == 2 ? PS2_CFG_PORT2_INT : PS2_CFG_PORT1_INT;
  cfg &= ~(ch == 2 ? PS2_CFG_PORT2_CLK : PS2_CFG_PORT1_CLK);

  ps2_ctrlr_cmd(PS2_CMD_WRITE_CFG);
  ps2_outb(PS2_DATA, cfg);

  ps2_dev_cmd(ch, PS2_DEV_CMD_SCAN_ENABLE);

  /*
    TODO : extract the following in separate functions
  */

  ps2_dev_cmd(ch, PS2_MOUSE_CMD_SAMPLE_RATE);
  
  if(ch == 2)
    ps2_ctrlr_cmd(PS2_CMD_PORT2_IN);

  ps2_outb(PS2_DATA, 80);
  
  ps2_dev_cmd(ch, PS2_MOUSE_CMD_RESOLUTION);
  
  if(ch == 2)
    ps2_ctrlr_cmd(PS2_CMD_PORT2_IN);
  ps2_outb(PS2_DATA, 0);

  irq_register(12, &mouse_event);
  pic_unmask(12);
}
