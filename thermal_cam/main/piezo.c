#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/ledc.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_timer.h"

#define LEDC_HS_TIMER          LEDC_TIMER_0
#define LEDC_HS_MODE           LEDC_HIGH_SPEED_MODE
#define LEDC_HS_CH0_CHANNEL    LEDC_CHANNEL_0
#define LEDC_HS_CH0_GPIO       (2)
#define LEDC_TEST_DUTY         (4000)
#define LEDC_TEST_FADE_TIME    (3000)

ledc_timer_config_t ledc_timer = {
    .duty_resolution = LEDC_TIMER_13_BIT, // resolution of PWM duty
    .freq_hz = 5000,                      // frequency of PWM signal
    .speed_mode = LEDC_HS_MODE,           // timer mode
    .timer_num = LEDC_HS_TIMER            // timer index
};

ledc_channel_config_t ledc_channel = {
    .channel    = LEDC_HS_CH0_CHANNEL,
    .duty       = 0,
    .gpio_num   = LEDC_HS_CH0_GPIO,
    .speed_mode = LEDC_HS_MODE,
    .hpoint     = 0,
    .timer_sel  = LEDC_HS_TIMER
};

void piezo_set_mute(bool mute){
  if(mute){
    ledc_set_duty(ledc_channel.speed_mode, ledc_channel.channel, 0x1fff);
  }
  else{
    ledc_set_duty(ledc_channel.speed_mode, ledc_channel.channel, 2000);
  }
  ledc_update_duty(ledc_channel.speed_mode, ledc_channel.channel);
}

void piezo_init(){
  ledc_timer_config(&ledc_timer);
  ledc_channel_config(&ledc_channel);
  ledc_fade_func_install(0);
  piezo_set_mute(false);
}

void piezo_play_frequency(uint32_t freq){
  ledc_set_freq(ledc_channel.speed_mode, ledc_channel.channel, freq);
  ledc_update_duty(ledc_channel.speed_mode, ledc_channel.channel);
}
