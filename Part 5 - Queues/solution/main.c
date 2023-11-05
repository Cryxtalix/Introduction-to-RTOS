#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
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
static const char *tag_a = "tag a";
static const char *tag_b = "tag b";
static const char *tag_i = "tag input";

// Queue
static const uint8_t queue_len = 5;
static QueueHandle_t queue1;
static QueueHandle_t queue2;
static QueueHandle_t input_queue;

static void taskA(void *params)
{
        char serial_input[128];
        char value[128];
        uint16_t delay;
        char blinked[50];

        while(1) {
                if (xQueueReceive(queue2, blinked, 0) == pdTRUE) {
                        printf("%s\n", blinked);
                }

                if (xQueueReceive(input_queue, serial_input, 0) == pdTRUE) {
                        printf("%s", serial_input);
                }

                if (strncmp(serial_input, "delay ", 6) == 0) {
                        strncpy(value, serial_input + 6, strlen(serial_input) - 6);

                        delay = atoi(value);
                        if (delay != 0) {
                                if (xQueueSend(queue1, &delay, 0) == pdTRUE) {
                                        ESP_LOGI(tag_a, "Wrote to delay queue.");
                                } else {
                                        ESP_LOGW(tag_a, "Unable to write to delay queue.");
                                }
                        }
                }
                memset(serial_input, 0, 128);

                vTaskDelay(333 / portTICK_PERIOD_MS);
        }
}

static void taskB(void *params)
{
        bool lcd = 0;
        uint16_t delay_time = 1000;
        uint32_t times_blinked = 0;
        const char *msg = "Blinked";

        while(1) {
                lcd = !lcd;
                gpio_set_level(PIN_LCD_BL, lcd);

                if (lcd == 1) {
                        times_blinked++;
                }

                if (times_blinked == 100) {
                        if (xQueueSend(queue2, msg, 0) == pdTRUE) {
                                ESP_LOGI(tag_b, "Wrote to blink queue.");
                        } else {
                                ESP_LOGW(tag_b, "Unable to write to blink queue.");
                        }
                        times_blinked = 0;
                }

                if (xQueueReceive(queue1, &delay_time, 0) == pdTRUE) {
                        ESP_LOGI(tag_b, "New delay time of %d.", delay_time);
                }

                vTaskDelay(delay_time / portTICK_PERIOD_MS);
        }
}

/*
* fgets() is blocking and has to have it's own thread
*/
void input_task(void *params)
{
        char serial_input[128];

        while(1) {
                if (fgets(serial_input, 128, stdin) != NULL) {
                        if (xQueueSend(input_queue, serial_input, 0) == pdFALSE) {
                                ESP_LOGW(tag_i, "Unable to write to input queue.");
                        }
                }
                vTaskDelay(500 / portTICK_PERIOD_MS);
        }
}

void app_main(void)
{
        esp32s3_serial_setup();

        gpio_reset_pin(PIN_LCD_BL);
        gpio_set_direction(PIN_LCD_BL, GPIO_MODE_OUTPUT);

        // Create queue
        queue1 = xQueueCreate(queue_len, sizeof(int));
        queue2 = xQueueCreate(queue_len, 50 * sizeof(char));
        input_queue = xQueueCreate(queue_len, 128 * sizeof(char));

        xTaskCreatePinnedToCore(                // Specific to esp32 freertos
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
                app_cpu);                       // Run in core 1 only
}