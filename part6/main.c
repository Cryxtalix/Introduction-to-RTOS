#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "semphr.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp32s3_setup.h"

// Use only core 1
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Log tags
//static const char *tag_a = "tag a";
//static const char *tag_b = "tag b";
//static const char *tag_i = "tag input";

// Queue
//static const uint8_t queue_len = 5;
//static QueueHandle_t queue1;

// Mutex
static SemaphoreHandle_t mutex;

static void taskA(void *params)
{
        while(1) {
                if (xSemaphoreTake(mutex, 0) == pdTRUE) {
                        // Do somethings
                        xSemaphoreGive(mutex);
                } else {
                        // Do something if mutex not available
                }

                vTaskDelay(333 / portTICK_PERIOD_MS);
        }
}

static void taskB(void *params)
{
        while(1) {

                vTaskDelay(500 / portTICK_PERIOD_MS);
        }
}

void app_main(void)
{
        esp32s3_serial_setup();

        gpio_reset_pin(PIN_LCD_BL);
        gpio_set_direction(PIN_LCD_BL, GPIO_MODE_OUTPUT);

        // Create muxtex
        mutex = xSemaphoreCreateMutex();

        /* xTaskCreatePinnedToCore(                // Specific to esp32 freertos
                taskA,                          // Function to be called
                "Task A",                       // Name of Task
                10000,                          // Stack size (bytes in esp32, words in freertos)
                NULL,                           // Parameter to pass to function
                configMAX_PRIORITIES,           // Task priority (0 to configMAX_PRIORITIES - 1)
                NULL,                           // Task handle 
                app_cpu);                       // Run in core 1 only

        xTaskCreatePinnedToCore(                // Specific to esp32 freertos
                taskB,                          // Function to be called
                "Task B",                       // Name of Task
                10000,                          // Stack size (bytes in esp32, words in freertos)
                NULL,                           // Parameter to pass to function
                configMAX_PRIORITIES - 1,       // Task priority (0 to configMAX_PRIORITIES - 1)
                NULL,                           // Task handle 
                app_cpu);                       // Run in core 1 only

        xTaskCreatePinnedToCore(                // Specific to esp32 freertos
                input_task,                     // Function to be called
                "Input task",                   // Name of Task
                10000,                          // Stack size (bytes in esp32, words in freertos)
                NULL,                           // Parameter to pass to function
                configMAX_PRIORITIES - 1,       // Task priority (0 to configMAX_PRIORITIES - 1)
                NULL,                           // Task handle 
                app_cpu);                       // Run in core 1 only */
}