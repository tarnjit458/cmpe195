#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_err.h"

void xThermal(void* p);

void app_main(){
  vTaskDelay(pdMS_TO_TICKS(100));
  printf("In main\n");
  vTaskDelay(pdMS_TO_TICKS(1000));
  xTaskCreatePinnedToCore(xThermal, "xThermal", 4096, NULL, 3, NULL, tskNO_AFFINITY);
  printf("Created a task\n");
}

void xThermal(void* p){
  printf("In xThermal\n");
  vTaskDelay(pdMS_TO_TICKS(1000));
  while(true){
    printf("Sent a frame\n");
  }
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
