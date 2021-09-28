#ifndef _PS2_H_
#define _PS2_H_

#define PS2_DATA    0x60
#define PS2_STATUS  0x64
#define PS2_CMD     0x64

#define PS2_STATUS_OUT          (1 << 0)
#define PS2_STATUS_IN           (1 << 1)
#define PS2_STATUS_SYS          (1 << 2)
#define PS2_STATUS_CMD          (1 << 3)
#define PS2_STATUS_TIMEOUT_ERR  (1 << 6)
#define PS2_STATUS_PARITY_ERR   (1 << 7)

#define PS2_CMD_READ_CFG                0x20
#define PS2_CMD_INTERNAL_RAM_READ(N)    (0x20 + N)
#define PS2_CMD_WRITE_CFG               0x60
#define PS2_CMD_INTERNAL_RAM_WRITE(N)   (0x60 + N)

#define PS2_CMD_CTRLR_TEST              0xAA
#define PS2_CTRLR_TEST_SUCCESS          0x55
#define PS2_CTRLR_TEST_FAIL             0xFC

#define PS2_CMD_PORT2_DISABLE           0xA7
#define PS2_CMD_PORT2_ENABLE            0xA8
#define PS2_CMD_PORT2_TEST              0xA9
#define PS2_CMD_PORT1_DISABLE           0xAD
#define PS2_CMD_PORT1_ENABLE            0xAE
#define PS2_CMD_PORT1_TEST              0xAB

#define PS2_TEST_SUCCESS                0x00
#define PS2_TEST_CLK_LOW                0x01
#define PS2_TEST_CLK_HIGH               0x02
#define PS2_TEST_DATA_LOW               0x03
#define PS2_TEST_DATA_HIGH              0x04

#define PS2_CMD_INTERNAL_RAM_DUMP       0xAC

#define PS2_CMD_IN_PORT_READ            0xC0
#define PS2_CMD_CPY_IN03_STATUS47       0xC1
#define PS2_CMD_CPY_IN47_STATUS47       0xC2

#define PS2_CMD_OUT_PORT_READ           0xD0
#define PS2_CMD_OUT_PORT_WRITE          0xD1

#define PS2_CMD_PORT1_OUT               0xD2
#define PS2_CMD_PORT2_OUT               0xD3
#define PS2_CMD_PORT2_IN                0xD4

#define PS2_CMD_PULSE(line)             (0xF0 & (line & 0xF))

#define PS2_CFG_PORT1_INT               (1 << 0)
#define PS2_CFG_PORT2_INT               (1 << 1)
#define PS2_CFG_SYS                     (1 << 2)
#define PS2_CFG_PORT1_CLK               (1 << 4)
#define PS2_CFG_PORT2_CLK               (1 << 5)
#define PS2_CFG_PORT1_TRANSLATION       (1 << 6)

#define PS2_CTRLR_OUT_SYS_RESET         (1 << 0)
#define PS2_CTRLR_OUT_A20               (1 << 1)
#define PS2_CTRLR_OUT_PORT2_CLK         (1 << 2)
#define PS2_CTRLR_OUT_PORT2_DATA        (1 << 3)
#define PS2_CTRLR_OUT_BUFFER_FULL1      (1 << 4)
#define PS2_CTRLR_OUT_BUFFER_FULL2      (1 << 5)
#define PS2_CTRLR_OUT_PORT1_CLK         (1 << 6)
#define PS2_CTRLR_OUT_PORT1_DATA        (1 << 7)

#define PS2_DEV_CMD_RESET               0xFF
#define PS2_DEV_CMD_IDENTIFY            0xF2
#define PS2_DEV_CMD_SCAN_ENABLE         0xF4
#define PS2_DEV_CMD_SCAN_DISABLE        0xF5
#define PS2_DEV_ACK                     0xFA
#define PS2_DEV_RESET_ACK               0xAA
#include <stdint.h>

uint8_t ps2_init();
uint8_t ps2_outb(uint16_t port, uint8_t data);
uint8_t ps2_inb(uint16_t port);
uint8_t ps2_ctrlr_cmd(uint8_t cmd);
uint8_t ps2_dev_cmd(uint8_t dev, uint8_t cmd);
uint8_t ps2_expect(uint8_t d);

#endif
