#ifndef ESP32S3
#define ESP32S3

#include <fcntl.h>
#include <stdio.h>
#include "driver/usb_serial_jtag.h"
#include "esp_vfs_usb_serial_jtag.h"
#include "esp_vfs_dev.h"

#define HEIGHT 320
#define WIDTH 170
#define LEFT_BTN 0
#define RIGHT_BTN 14
#define PIN_POWER_ON 15
#define PIN_LCD_BL 38

void esp32s3_serial_setup(void);

#endif //ESP32S3