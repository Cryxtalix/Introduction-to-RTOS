#include "tdisplays3.h"

// Timer handle
static TimerHandle_t one_shot_timer = NULL;

// Callbacks
void timer_callback(TimerHandle_t xtimer)
{
        // Cast timer ID back to an int
        printf("%"PRIu32": Timer expired...\n", (uint32_t)pvTimerGetTimerID(xtimer));
}

void app_main(void)
{
        vTaskDelay(1000/portTICK_PERIOD_MS);
        printf("ESP32s3 started...\n");

        one_shot_timer = xTimerCreate (         // Create timer
                "One-shot timer",               // Name
                2000 / portTICK_PERIOD_MS,      // Period of timer (ticks)
                pdFALSE,                        // Auto-reload
                (void *)0,                      // Timer ID (Must cast uint32 to void pointer)
                timer_callback                  // Callback function
        );

        // xTimerCreate uses heap memory, and has a potential to fail.
        if (one_shot_timer == NULL) {
                printf("Can't create timer...\n");
        } else {
                printf("Start timers...\n");

                // Commands send to timer command queue, so max block time needs to be specified
                xTimerStart(one_shot_timer, portMAX_DELAY);
        }
}
