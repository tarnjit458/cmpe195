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

//Note: Thermal sensor register addresses and data are 16 bits!
//      This seems pretty nonstandard but it should work fine

//Multibyte I2C read.
int MLX90640_I2CRead( //The address of the sensor
                      uint8_t slaveAddr,
                      //The starting address of the read
                      uint16_t startAddress,
                      //The amount of bytes to read from the start
                      uint16_t nMemAddressRead,
                      //An array to store the data
                      uint16_t *data);

//Single Byte I2C Write
int MLX90640_I2CWrite(
                      //The address of the sensor
                      uint8_t slaveAddr,
                      //The address of the write
                      uint16_t writeAddress,
                      //The data to write
                      uint16_t data);

void app_main(){
  init_i2c();
}
//TODO: Rework this to use the wierd 16 bit format for the sensor. See if we can use i2c_master_read instead of i2c_master_read_byte
//Multibyte I2C read.
int MLX90640_I2CRead(uint8_t slaveAddr, uint16_t startAddress, uint16_t nMemAddressRead, uint16_t *data){
  //Init the i2c handle
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  //Queue the START command
  i2c_master_start(cmd);
  //Queue the device address shifted to the left, indicate we want to write (0), enable the ACK check
  i2c_master_write_byte(cmd, (slave_addr << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
  //Queue the register address we want, with an ACK check enabled
  i2c_master_write_byte(cmd, slave_reg, ACK_CHECK_EN);
  //Queue the repeated START command
  i2c_master_start(cmd);
  //Queue the device address shifted to the left, indicate we want to read (1), enable the ACK check
  i2c_master_write_byte(cmd, (slave_addr << 1) | I2C_MASTER_READ, ACK_CHECK_EN);
  //Begin repeated reads. Only go through this loop if reading more than twice
  uint16_t i = 1;
  while(i < nMemAddressRead){
    //Queue reading the data off the bus into our buffer, Send an ack
    i2c_master_read_byte(cmd, dest + i - 1, I2C_MASTER_ACK);
    i++
  }
  //Read out the last (or only) byte, send a NACK to stop the transaction
  i2c_master_read_byte(cmd, nMemAddressRead - 1, I2C_MASTER_NACK);
  //Queue the STOP command
  i2c_master_stop(cmd);
  //Perform the queued i2c command
  esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
  //Delete the i2c link
  i2c_cmd_link_delete(cmd);
  //Return the result of the transaction
  return ret;
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
*/
