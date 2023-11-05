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

void do_something(void *parameter) 
{
        while(1) {
                int a = 1;
                int b[100];

                for (int i = 0; i < 100; i++) {
                        b[i] = a + i;
                }
                printf("%d\n", b[0]);
                printf("Size of int: %d\n", sizeof(int));

                printf("High water mark (words): %d\n", uxTaskGetStackHighWaterMark(NULL));
                printf("Heap before malloc (bytes): %d\n", xPortGetFreeHeapSize());

                int *ptr = (int*)pvPortMalloc(1024 * sizeof(int));

                if (ptr == NULL) {
                        printf("Out of heap!\n");
                } else {
                        for (int i = 0; i < 1024; i++) {
                                ptr[i] = 3;
                        }
                }

                //After malloc
                printf("Heap after malloc (bytes): %d\n", xPortGetFreeHeapSize());

                vPortFree(ptr);

                vTaskDelay(100 / portTICK_PERIOD_MS);
        }
}

void app_main(void)
{
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        char *ourTaskName = pcTaskGetName(NULL);
        ESP_LOGI(ourTaskName, "Hello, starting up!\n");

        // If this was vanilla freertos, we would want to call vTaskStartScheduler() in main
        // after setting up the tasks. Arduino setup function calls it automatically.

        xTaskCreatePinnedToCore(        // Specific to esp32 freertos
                do_something,           // Function to be called
                "Doing something",      // Name of Task
                2400,                   // Stack size (bytes in esp32, words in freertos)
                NULL,                   // Parameter to pass to function
                1,                      // Task priority (0 to configMAX_PRIORITIES - 1)
                NULL,                   // Task handle 
                app_cpu);               // Run in core 1 only
}