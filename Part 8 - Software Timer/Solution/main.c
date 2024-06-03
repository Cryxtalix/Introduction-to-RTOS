#include "tdisplays3.h"

// Timer handle
static TimerHandle_t my_timer = NULL;

// Callbacks
void timer_callback(TimerHandle_t xtimer)
{
        // Turn off LCD
        gpio_set_level(PIN_LCD_BL, 0);
}

void input_task(void *params)
{
        int buffer;

        while(1) {
                buffer = fgetc(stdin);
                if (buffer != EOF) {
                        // Turn on LCD
                        gpio_set_level(PIN_LCD_BL, 1);

                        printf("%c", (char)buffer);
                        fsync(fileno(stdout));

                        if (my_timer != NULL) {
                                xTimerStart(my_timer, portMAX_DELAY);
                        }
                }

                vTaskDelay(10 / portTICK_PERIOD_MS);
        }
}

void app_main(void)
{
        vTaskDelay(1000/portTICK_PERIOD_MS);
        printf("ESP32s3 started...\n");

        gpio_reset_pin(PIN_LCD_BL);
        gpio_set_direction(PIN_LCD_BL, GPIO_MODE_OUTPUT);

        my_timer = xTimerCreate (               // Create timer
                "Timer",                        // Name
                5000 / portTICK_PERIOD_MS,      // Period of timer (ticks)
                pdFALSE,                        // Auto-reload
                (void *)0,                      // Timer ID (Must cast uint32 to void pointer)
                timer_callback                  // Callback function
        );

        xTaskCreatePinnedToCore(input_task, "input task", 10000, NULL, 1, NULL, app_cpu);
}
