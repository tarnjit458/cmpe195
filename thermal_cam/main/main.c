#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//#include "freertos/queue.h"
#include "sdkconfig.h"
//#include "esp_timer.h"
//#include "driver/gpio.h"
#include "driver/i2c.h"

#include "i2c.h"
#include "screen.h"
//#include "therm.h"
//#include "interp.h"
#include "MLX90640_I2C_Driver.h"
#include "MLX90640_API.h"


#define SCALE_FACTOR  2
#define THERM_RES_X   32
#define THERM_RES_Y   24
#define INTERP_RES_X  96
#define INTERP_RES_Y  128
#define SLAVE_ADDR    0x33

static float* therm_buf;
void xThermal(void* p);
void xExtractParameters(void* p);

void app_main(){
  printf("In main\n");
  xTaskCreate(xThermal, "xThermal", 16384, NULL, 2, NULL);
  printf("Created a task\n");
  vTaskStartScheduler();
}

void xThermal(void* p){
  uint16_t *fbuf = heap_caps_malloc(SCREEN_WIDTH*SCREEN_HEIGHT*sizeof(uint16_t), MALLOC_CAP_DMA);
  printf("In xThermal\n");
  init_i2c();
  unsigned char slaveAddress = 0x33;
  uint16_t* eeMLX90640 = malloc(sizeof(uint16_t)*832);
  static uint16_t mlx90640Frame[834];
  paramsMLX90640 mlx90640;
  static float mlx90640Image[768];
  printf("DumpEE status: %i\n", MLX90640_DumpEE(slaveAddress, eeMLX90640));
  printf("ExtractParameters status: %i\n", MLX90640_ExtractParameters(eeMLX90640, &mlx90640));
  free(eeMLX90640);
  uint8_t mode = MLX90640_GetCurMode(0x33);
  if(mode){
    printf("Mode: Chess Pattern\n");
  }
  else{
    printf("Mode: Interleaved");
  }
  while(1){
    MLX90640_GetFrameData (0x33, mlx90640Frame);
    MLX90640_GetImage(mlx90640Frame, &mlx90640, mlx90640Image);
    for(uint8_t i = 0; i < 24; i++){
      for(uint8_t j = 0; j < 32; j++){
        printf("%.0f ", (mlx90640Image[i*j]/10000000+ 100));
      }
      printf("\n");
    }
    printf("\n\n\n");
  };
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
