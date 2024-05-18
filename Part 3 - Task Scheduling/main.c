#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/uart.h"

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

// Task handles
static TaskHandle_t task1 = NULL;
static TaskHandle_t task2 = NULL;

const char *msg = "111111111111111111111111111111";

void print_str(void *parameter) 
{
        while(1) {
                for (int i = 0; i < strlen(msg); i++) {
                        printf("%c", msg[i]);
                        vTaskDelay(10 / portTICK_PERIOD_MS);
                };
                vTaskDelay(1000 / portTICK_PERIOD_MS);
        };
}

void print_ast(void *parameter) 
{
        while(1) {
                printf("%c", '*');
                vTaskDelay(115 / portTICK_PERIOD_MS);
        };
}

void loop(void *parameter)
{
        while(1) {
                for (int i = 0; i < 3; i++) {
                        vTaskSuspend(task2);
                        vTaskDelay(5000 / portTICK_PERIOD_MS);
                        vTaskResume(task2);
                        vTaskDelay(5000 / portTICK_PERIOD_MS);
                };

                // Check if task handle is NULL, then set to NULL immediately after deletion
                // Calling vTaskDelete on non existent handler is UB
                if (task1 != NULL) {
                        vTaskDelete(task1);
                        task1 = NULL;
                }
        };
}

void app_main(void)
{
        char *ourTaskName = pcTaskGetName(NULL);
        ESP_LOGI(ourTaskName, "Hello, starting up!\n");

        //gpio_reset_pin(PIN_LCD_BL);
        //gpio_set_direction(PIN_LCD_BL, GPIO_MODE_OUTPUT);

        xTaskCreatePinnedToCore( // Specific to esp32 freertos
                print_str,       // Function to be called
                "print_str",    // Name of Task
                2048,            // Stack size (bytes in esp32, words in freertos)
                NULL,            // Parameter to pass to function
                1,               // Task priority (0 to configMAX_PRIORITIES - 1)
                &task1,            // Task handle 
                app_cpu          // Run in core 1 only
        );

        xTaskCreatePinnedToCore( // Specific to esp32 freertos
                print_ast,      // Function to be called
                "print_ast",    // Name of Task
                2048,            // Stack size (bytes in esp32, words in freertos)
                NULL,            // Parameter to pass to function
                2,               // Task priority (0 to configMAX_PRIORITIES - 1)
                &task2,            // Task handle 
                app_cpu          // Run in core 1 only
        );

        xTaskCreatePinnedToCore( // Specific to esp32 freertos
                loop,      // Function to be called
                "looping",    // Name of Task
                2048,            // Stack size (bytes in esp32, words in freertos)
                NULL,            // Parameter to pass to function
                10,               // Task priority (0 to configMAX_PRIORITIES - 1)
                NULL,            // Task handle 
                app_cpu          // Run in core 1 only
        );

        // If this was vanilla freertos, we would want to call vTaskStartScheduler() in main
        // after setting up the tasks. ESP-IDF setup function calls it automatically.
}