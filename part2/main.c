#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"

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
#endif

// LED rates
static const int rate_1 = 500;  // ms
static const int rate_2 = 323;  // ms

void toggleLED(void *parameter) 
{
        while(1) {
                gpio_set_level(PIN_LCD_BL, 1);
                vTaskDelay(rate_1 / portTICK_PERIOD_MS); // Expects number of ticks to delay
                gpio_set_level(PIN_LCD_BL, 0);
                vTaskDelay(rate_1 / portTICK_PERIOD_MS); // Divide 500ms by time of each tick
        }
}

void fast_blink(void *parameter) 
{
        while(1) {
                gpio_set_level(PIN_LCD_BL, 1);
                vTaskDelay(rate_2 / portTICK_PERIOD_MS); // Expects number of ticks to delay
                gpio_set_level(PIN_LCD_BL, 0);
                vTaskDelay(rate_2 / portTICK_PERIOD_MS); // Divide 500ms by time of each tick
        }
}

void app_main(void)
{
        char *ourTaskName = pcTaskGetName(NULL);
        ESP_LOGI(ourTaskName, "Hello, starting up!\n");

        gpio_reset_pin(PIN_LCD_BL);
        gpio_set_direction(PIN_LCD_BL, GPIO_MODE_OUTPUT);

        xTaskCreatePinnedToCore( // Specific to esp32 freertos
                toggleLED,       // Function to be called
                "Toggle LED",    // Name of Task
                1024,            // Stack size (bytes in esp32, words in freertos)
                NULL,            // Parameter to pass to function
                1,               // Task priority (0 to configMAX_PRIORITIES - 1)
                NULL,            // Task handle 
                app_cpu);        // Run in core 1 only

        // If this was vanilla freertos, we would want to call vTaskStartScheduler() in main
        // after setting up the tasks. Arduino setup function calls it automatically.

        xTaskCreatePinnedToCore( // Specific to esp32 freertos
                fast_blink,       // Function to be called
                "Fast blink",    // Name of Task
                1024,            // Stack size (bytes in esp32, words in freertos)
                NULL,            // Parameter to pass to function
                1,               // Task priority (0 to configMAX_PRIORITIES - 1)
                NULL,            // Task handle 
                app_cpu);        // Run in core 1 only
}