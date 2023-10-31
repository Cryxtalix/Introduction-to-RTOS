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
static const char *tag_a = "tag_a";
static const char *tag_b = "tag_b";

// Task handles
static TaskHandle_t handle_a = NULL;
static TaskHandle_t handle_b = NULL;

char *str_buf = NULL;
bool notice = false;

static void task_a(void *params)
{
        char input_buffer[2];
        int str_buf_len = 10;

        while (1) {
                input_buffer[0] = fgetc(stdin);
                input_buffer[1] = '\0';

                if(str_buf == NULL) {
                        str_buf_len = 10;
                        str_buf = calloc(str_buf_len, sizeof(char));
                        if (str_buf == NULL) {
                                ESP_LOGE(tag_a, "Out of memory");
                                esp_restart();
                        }
                }
                
                if (strlen(str_buf) == str_buf_len - 1) {
                        str_buf_len += 10;
                        str_buf = realloc(str_buf, str_buf_len * sizeof(char));
                        if(str_buf == NULL) {
                                ESP_LOGE(tag_a, "Out of memory");
                                esp_restart();
                        }
                }

                strncat(str_buf, input_buffer, 2);

                if(input_buffer[0] == '\n') {
                        notice = true;
                }
                vTaskDelay(50 / portTICK_PERIOD_MS);
        }
}

static void task_b(void *params)
{
        while(1) {
                if(notice == true) {
                        printf("%s", str_buf);
                        free(str_buf);
                        str_buf = NULL;
                        notice = false;
                        ESP_LOGI(tag_b, "Heap remaining: %d", xPortGetFreeHeapSize());
                }
                vTaskDelay(50 / portTICK_PERIOD_MS);
        }
}

void app_main(void)
{
        esp32s3_serial_setup();

        xTaskCreatePinnedToCore(                // Specific to esp32 freertos
                task_a,                         // Function to be called
                "Task A",                       // Name of Task
                2000,                           // Stack size (bytes in esp32, words in freertos)
                NULL,                           // Parameter to pass to function
                configMAX_PRIORITIES - 1,       // Task priority (0 to configMAX_PRIORITIES - 1)
                &handle_a,                      // Task handle 
                app_cpu);                       // Run in core 1 only

        xTaskCreatePinnedToCore(                // Specific to esp32 freertos
                task_b,                         // Function to be called
                "Task B",                       // Name of Task
                2500,                           // Stack size (bytes in esp32, words in freertos)
                NULL,                           // Parameter to pass to function
                configMAX_PRIORITIES,           // Task priority (0 to configMAX_PRIORITIES - 1)
                &handle_b,                      // Task handle 
                app_cpu);                       // Run in core 1 only
}