#include "tdisplays3.h"

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
