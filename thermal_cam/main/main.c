#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "i2c.h"
#include "screen.h"
#include "therm.h"
#include "interp.h"

#define SCALE_FACTOR  10
#define THERM_RES     8
#define INTERP_RES    24

static float* therm_buf;

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
    /*
    for(uint8_t i = 0; i < THERM_RES; i++){
      printf("\n");
      for(uint8_t j = 0; j < THERM_RES; j++){
        printf("%f ", therm_buf[i*THERM_RES + j]);
      }
    }*/
    interpolate_image(therm_buf, THERM_RES, THERM_RES, ibuf, INTERP_RES, INTERP_RES);
    /*
    for(uint8_t i = 0; i < INTERP_RES; i++){
      printf("\n");
      for(uint8_t j = 0; j < INTERP_RES; j++){
        printf("%f ", ibuf[i*INTERP_RES + j]);
      }
    }
    */
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
