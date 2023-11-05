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

// Queue
static const uint8_t queue_len = 5;
static QueueHandle_t msg_queue;

static void print_msg(void *params)
{
        int item;

        while(1) {
                if (xQueueReceive(
                        msg_queue,                      // Queue handle
                        (void *)&item,                  // Function expects a void*, but casting is optional
                        0                               // Number of ticks to wait in case nothing arrives from the queue
                        ) == pdTRUE) {
                                //printf("Int received: %d\n", item);
                }

                printf("Int received: %d\n", item);

                vTaskDelay(500 / portTICK_PERIOD_MS);
        }
}

static void send_queue(void *params)
{
        while(1) {
                static int num = 0;
                if (xQueueSend(msg_queue, (void *)&num, 10) != pdTRUE) {
                        ESP_LOGW(tag_a, "Queue full");
                }
                num++;

                vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
}

void app_main(void)
{
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        esp32s3_serial_setup();

        // Create queue
        msg_queue = xQueueCreate(queue_len, sizeof(int));

        xTaskCreatePinnedToCore(                // Specific to esp32 freertos
                print_msg,                      // Function to be called
                "Print msg",                    // Name of Task
                4000,                           // Stack size (bytes in esp32, words in freertos)
                NULL,                           // Parameter to pass to function
                configMAX_PRIORITIES - 1,       // Task priority (0 to configMAX_PRIORITIES - 1)
                NULL,                           // Task handle 
                app_cpu);                       // Run in core 1 only

        xTaskCreatePinnedToCore(                // Specific to esp32 freertos
                send_queue,                     // Function to be called
                "Send to queue",                // Name of Task
                4000,                           // Stack size (bytes in esp32, words in freertos)
                NULL,                           // Parameter to pass to function
                configMAX_PRIORITIES - 1,       // Task priority (0 to configMAX_PRIORITIES - 1)
                NULL,                           // Task handle 
                app_cpu);                       // Run in core 1 only
}