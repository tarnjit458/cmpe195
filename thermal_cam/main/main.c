#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/ledc.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "i2c.h"
#include "screen.h"
#include "therm.h"
#include "interp.h"
#include "piezo.c"

#define SCALE_FACTOR  10
#define THERM_RES     8
#define INTERP_RES    24
#define LED0          12
#define LED1          4

static float* therm_buf;

void app_main(){
  //Initialize all the GPIO for the LEDs
  gpio_pad_select_gpio(LED0);
  gpio_pad_select_gpio(LED1);
  gpio_set_direction(LED0, GPIO_MODE_OUTPUT);
  gpio_set_direction(LED1, GPIO_MODE_OUTPUT);
  gpio_set_level(LED0, 1);
  gpio_set_level(LED1, 1);
  //Init the piezo and play a startup sound
  piezo_init();
  piezo_play_frequency(1000);
  vTaskDelay(150 / portTICK_PERIOD_MS);
  piezo_play_frequency(2600);
  vTaskDelay(100 / portTICK_PERIOD_MS);
  piezo_set_mute(true);
  //Init the thermal camera
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
