#include <drivers/ps2.h>
#include <io/io.h>
#include <util/logger.h>
#include <multitasking/schedlock.h>

uint8_t ps2_channels[2] = {0};

uint8_t ps2_outb(uint16_t port, uint8_t data){
  uint32_t t = 512;
  
  while ((io_inb(PS2_STATUS) & PS2_STATUS_IN) && t --> 0){
    asm volatile("pause");
  }
  
  if(t == 0){
    log_f(ERROR, "PS2", "Write failed");
    return 0xFF;
  }
  
  io_outb(port, data);
  return 0;
}

uint8_t ps2_inb(uint16_t port){
  uint32_t t = 512;

  while ((io_inb(PS2_STATUS) & PS2_STATUS_OUT) && t --> 0){
    asm volatile("pause");
  }
  
  if(t == 0){
    log_f(ERROR, "PS2", "Read failed");
    return 0xFF;
  }
  return io_inb(port);
}


inline uint8_t ps2_ctrlr_cmd(uint8_t cmd){
  return ps2_outb(PS2_CMD, cmd);
}

inline uint8_t ps2_dev_cmd(uint8_t dev, uint8_t cmd){
  uint32_t t = 20, result = 0;
  do {
    if(dev == 2)
      ps2_ctrlr_cmd(PS2_CMD_PORT2_IN);
    ps2_outb(PS2_DATA, cmd);
    result = ps2_inb(PS2_DATA);
    //log_f(WARN, "PS/2", "dev_cmd %d => received %d", cmd, result);

  } while ((result != PS2_DEV_ACK) && t-- > 0);
  
  return t != 0;
}

inline uint8_t ps2_dev_cmd_expect_nothing(uint8_t dev, uint8_t cmd){
  if(dev == 2)
      ps2_ctrlr_cmd(PS2_CMD_PORT2_IN);
  return ps2_outb(PS2_DATA, cmd);
}



inline uint8_t ps2_expect(uint8_t d){
  uint8_t r = ps2_inb(PS2_DATA);
  if(r != d)
    log_f(WARN, "PS/2", "Expected %d, received %d", d, r);
  return r;
}

static void ps2_test_failed(uint8_t p, uint8_t r){
  switch(r){
  case PS2_TEST_CLK_LOW:
    log_f(ERROR, "PS2", "Port %d clock is stuck on low", p);
    break;
  case PS2_TEST_CLK_HIGH:
    log_f(ERROR, "PS2", "Port %d clock is stuck on high", p);
    break;
  case PS2_TEST_DATA_LOW:
    log_f(ERROR, "PS2", "Port %d data is stuck on low", p);
    break;
  case PS2_TEST_DATA_HIGH:
    log_f(ERROR, "PS2", "Port %d data is stuck on high", p);
    break;
  default:
    log_f(ERROR, "PS2", "??");
    break;
  }
}

char *ps2_device_names[] = {
  "Unknown",
  "Ancient AT keyboard with translation enabled",
  "Standard PS/2 mouse",
  "Mouse with scroll wheel",
  "MF2 keyboard with translation enabled",
  "MF2 keyboard",
  "PS/2 Mouse",
  "PS/2 Keyboard",
};

typedef void (*ps2_device_driver_t)(uint8_t);

#include <drivers/kbd.h>
#include <drivers/mouse.h>

ps2_device_driver_t ps2_device_drivers[] = {
  0,
  &kbd_init,
  &mouse_init,
  &mouse_init,
  &kbd_init,
  &kbd_init,
  &mouse_init,
  &kbd_init,
};

/* There are probably mistakes here */
static uint8_t ps2_device_id(uint16_t w){
  switch(w){
    case 0xFFFF: return 1;
    case 0x00FF: return 2; 
    case 0x03FF: return 3;
    case 0x41AB:
    case 0xC1AB: return 4; 
    case 0x83AB: return 5;
    case 0x0000: return 7;
    
    default: 
    {
      switch((w & 0xFF) >> 8){
        case 0x0:
        case 0x3:
        case 0x4: return 6;
        default: return 0;
      }
    }
  }
}

static inline char *ps2_device_name(uint16_t w){
  uint8_t id = ps2_device_id(w);
  return ps2_device_names[id];
}

static inline ps2_device_driver_t ps2_device_driver(uint16_t w){
  return ps2_device_drivers[ps2_device_id(w)];
}

static uint8_t ps2_initialize_present_devices(){
  for(uint8_t i = 0; i < 2; i++){
    if(!ps2_channels[i]) continue;
    
    uint8_t ch = i + 1;
    ps2_ctrlr_cmd(ch == 1? PS2_CMD_PORT1_ENABLE: PS2_CMD_PORT2_ENABLE);
    
    ps2_dev_cmd(ch, PS2_DEV_CMD_SCAN_DISABLE);
    ps2_dev_cmd(ch, PS2_DEV_CMD_IDENTIFY);
     
    uint8_t b1 = ps2_inb(PS2_DATA);
    uint8_t b2 = ps2_inb(PS2_DATA);
  
    uint16_t w = b1 | (b2 << 8);

    log_info(NICE_BLUE, "PS/2", "Device detected : %s", ps2_device_name(w));

    ps2_device_driver_t init = ps2_device_driver(w);
    if(init)
      init(ch);
    
  }

  return 0;
}

uint8_t ps2_init(){
  log_info(NICE_BLUE_0, "PS/2", "Detecting and initializing PS/2 devices");
  
  uint32_t ef = get_eflags_and_cli();

  /* TODO: check if the PS/2 controller exists */

  /* Disable both ports */
  ps2_ctrlr_cmd(PS2_CMD_PORT1_DISABLE);
  ps2_ctrlr_cmd(PS2_CMD_PORT2_DISABLE);
  
  /* Empty the buffer */
  ps2_inb(PS2_DATA);

  /* Clear the interrupt flags and the translation one */
  ps2_ctrlr_cmd(PS2_CMD_READ_CFG);
  uint8_t cfg = ps2_inb(PS2_DATA);

  /* By the way, check whether it is dual channel */
  uint8_t dual = !(cfg & PS2_CFG_PORT2_CLK);
  
  cfg &= ~(PS2_CFG_PORT1_INT | PS2_CFG_PORT2_INT | PS2_CFG_PORT1_TRANSLATION);

  ps2_ctrlr_cmd(PS2_CMD_WRITE_CFG);
  ps2_outb(PS2_DATA, cfg);
  
  /* Run the controller test */
  ps2_ctrlr_cmd(PS2_CMD_CTRLR_TEST);
  uint8_t ctrlr_test_result = ps2_inb(PS2_DATA);
  
  if(ctrlr_test_result == PS2_CTRLR_TEST_FAIL){
    log_f(ERROR, "PS/2", "Controller test failed");
    return 1;
  }
  
  /* Restore the config byte just in case the test ruined it */
  ps2_ctrlr_cmd(PS2_CMD_WRITE_CFG);
  ps2_outb(PS2_DATA, cfg);  
  

  if(!dual){
    ps2_ctrlr_cmd(PS2_CMD_PORT2_ENABLE);
    ps2_ctrlr_cmd(PS2_CMD_READ_CFG);
    uint8_t cfg = ps2_inb(PS2_DATA);

    dual = !(cfg & PS2_CFG_PORT2_CLK);
    ps2_ctrlr_cmd(PS2_CMD_PORT2_DISABLE);

  }

  log_info(NICE_BLUE_0, "PS/2", "The controller has %s", dual ? "2 channels" : "1 channel");

  ps2_ctrlr_cmd(PS2_CMD_PORT1_TEST);
  uint8_t port1_test_result = ps2_inb(PS2_DATA);
  
  if(port1_test_result != PS2_TEST_SUCCESS){
    ps2_test_failed(1, port1_test_result);
  }else{
    ps2_channels[0] = 1;
  }

  if(dual){
    ps2_ctrlr_cmd(PS2_CMD_PORT2_TEST);
    uint8_t port2_test_result = ps2_inb(PS2_DATA);
    if(port2_test_result != PS2_TEST_SUCCESS){
      ps2_test_failed(2, port1_test_result);
    } else {
      ps2_channels[1] = 1;
    }
  }
  
  /* Reset present devices */
  for(uint8_t i = 0; i < 2; i++){
    if(!ps2_channels[i]) continue;
    uint8_t ch = i + 1;

    ps2_ctrlr_cmd(ch == 1 ? PS2_CMD_PORT1_ENABLE : PS2_CMD_PORT2_ENABLE);

    ps2_dev_cmd_expect_nothing(ch, PS2_DEV_CMD_RESET);
    
    if(ch == 1){
      ps2_expect(PS2_DEV_ACK);
      continue;
    }

    uint8_t A0 = ps2_inb(PS2_DATA);
    uint8_t A1 = ps2_inb(PS2_DATA);
    
    ps2_ctrlr_cmd(ch==1?PS2_CMD_PORT1_DISABLE:PS2_CMD_PORT2_DISABLE);
    
    uint8_t ack_success = (A0 == PS2_DEV_ACK && A1 == PS2_DEV_RESET_ACK)
                        || (A1 == PS2_DEV_ACK && A0 == PS2_DEV_RESET_ACK)
                        || (A0 == PS2_DEV_RESET_ACK && (A1 == PS2_DEV_ACK || A1 == 0))
                        || ((A0 == PS2_DEV_ACK || A0 == 0x00) && A1 == PS2_DEV_RESET_ACK);
    if(ch == 1 && !ack_success)
      ack_success = A0 == PS2_DEV_ACK;
      
    
    if(!ack_success)
      ps2_channels[i] = 0;
  }

  uint8_t r = ps2_initialize_present_devices();
  
  set_eflags(ef);

  return r;
}

