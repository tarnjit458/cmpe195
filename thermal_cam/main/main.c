#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "sdkconfig.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "driver/i2c.h"

#include "i2c.h"
#include "screen.h"
#include "therm.h"
#include "interp.h"
#include "MLX90640_I2C_Driver.h"
#include "MLX90640_API.h"


#define SCALE_FACTOR  10
#define THERM_RES     8
#define INTERP_RES    24

static float* therm_buf;

void app_main(){
  init_i2c();
  uint16_t dat[4] ={0, 0, 0, 0};
  //Read back known data
  //Should get:
  //0x240C = 0x1901
  //0x240D = 0x0000
  //0x240E = 0x0000
  //0x240F = 0xBE33
  MLX90640_I2CRead(0x33, 0x240C, 4, dat);
  for(uint8_t i = 0; i < 4; i++){
    printf("dat[%i]: 0x%x\n", i, dat[i]);
  }
  //Extract the device eeprom
  uint16_t eeMLX90640[832];
  paramsMLX90640 mlx90640;
  printf("DumpEE Status: %i\n", MLX90640_DumpEE(0x33, eeMLX90640));
  //Set the framerate
  uint8_t slave_addr = 0x33;
  uint8_t refresh_rate = 0x06;
  printf("Setting refresh rate to %i...\n", refresh_rate);
  MLX90640_SetRefreshRate(slave_addr, refresh_rate);
  printf("Refresh rate is now %i.\n", MLX90640_GetRefreshRate(slave_addr));
  printf("EEPROM check returns %i\n", CheckEEPROMValid(eeMLX90640));
  // printf("Dumping EEPROM Data...\n");
  // for(uint16_t i = 0; i < 832; i++){
  //  printf("%i: 0x%x\n", i, eeMLX90640[i]);
  // }
  //TODO: Put this in its own task, give it a fuckload of RAM
  ExtractVDDParameters(eeMLX90640, &mlx90640);
}

/*
void app_main(){
  init_i2c();
  therm_buf = malloc(sizeof(float)* THERM_RES*THERM_RES);
  //Init the SPI screen
  spi_device_handle_t screen_spi = lcd_spi_init();
  lcd_screen_init(screen_spi);
  //Allocate a DMA framebuffer
  uint16_t *fbuf = heap_caps_malloc(SCREEN_WIDTH*SCREEN_HEIGHT*sizeof(uint16_t), MALLOC_CAP_DMA);
  //Allocate a buffer for interpolation
  float *ibuf = malloc(INTERP_RES*INTERP_RES*sizeof(float));
  while(1){

    therm_read_frame_float(therm_buf);
    interpolate_image(therm_buf, THERM_RES, THERM_RES, ibuf, INTERP_RES, INTERP_RES);
    for(uint16_t y = 0; y < SCREEN_HEIGHT; y++){
      for(uint16_t x = 0; x < SCALE_FACTOR*INTERP_RES; x++){
        fbuf[(y * SCREEN_WIDTH) + x] = therm_colors[(uint16_t)(ibuf[((y/SCALE_FACTOR)*INTERP_RES + (x/SCALE_FACTOR))])];
      }
      for(uint16_t b = SCALE_FACTOR*INTERP_RES; b < SCREEN_WIDTH; b++){
        fbuf[(y * SCREEN_WIDTH) + b] = 0x0;
      }
    }
    lcd_send_fbuff(screen_spi, fbuf);
  }
}
*/
