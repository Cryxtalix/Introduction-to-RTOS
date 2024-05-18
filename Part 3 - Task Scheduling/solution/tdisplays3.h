#ifndef TDISPS3
#define TDISPS3

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/uart.h"

// Pins specific to the TDisplayS3 Chip
#define HEIGHT 320
#define WIDTH 170
#define LEFT_BTN 0
#define RIGHT_BTN 14
#define PIN_POWER_ON 15
#define PIN_LCD_BL 38

// Use only core 1
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif // CONFIG_FREERTOS_UNICORE

#endif // TDISPS3