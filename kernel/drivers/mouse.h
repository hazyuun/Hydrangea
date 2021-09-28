#ifndef _MOUSE_H_
#define _MOUSE_H_

/*
  Nobody :
  My mind at almost 5 AM : Let's make a PS/2 Mouse driver for no reason
  ME : Ok ._. 
*/

#include <stdint.h>

#define MOUSE_PACKET_BTN_LEFT    (1 << 0)
#define MOUSE_PACKET_BTN_RIGHT   (1 << 1)
#define MOUSE_PACKET_BTN_MIDDLE  (1 << 2)
#define MOUSE_PACKET_ONE         (1 << 3)
#define MOUSE_PACKET_X_SIGN      (1 << 4)
#define MOUSE_PACKET_Y_SIGN      (1 << 5)
#define MOUSE_PACKET_X_OVERFLOW  (1 << 6)
#define MOUSE_PACKET_Y_OVERFLOW  (1 << 7)

#define PS2_MOUSE_CMD_RESEND              0xFE
#define PS2_MOUSE_CMD_DEFAULT             0xF6
#define PS2_MOUSE_CMD_SAMPLE_RATE         0xF3
#define PS2_MOUSE_CMD_MOUSEID             0xF2
#define PS2_MOUSE_CMD_REQ_SINGLE_PACKET   0xEB
#define PS2_MOUSE_CMD_STATUS              0xE9
#define PS2_MOUSE_CMD_RESOLUTION          0xE8

void mouse_init(uint8_t ch);
#endif