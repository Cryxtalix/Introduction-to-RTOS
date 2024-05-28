#include "tdisplays3.h"

static SemaphoreHandle_t bin_sem; // Switch to binary semaphore instead

static void receive(void *params)
{
        char my_char = *(char *)params;
        xSemaphoreGive(bin_sem); // App_main can return after this line
        printf("Task received: %c\n", my_char);
        vTaskDelete(NULL);
}

void app_main(void)
{
        bin_sem = xSemaphoreCreateBinary();

        int int_buf;
        char char_buf;

        while(1) {
                int_buf = fgetc(stdin);
                if (int_buf != EOF) {
                        char_buf = (char)int_buf;
                        break;
                }
        }

        printf("User entered: %c or %i\n", char_buf, int_buf);

        xTaskCreatePinnedToCore(receive, "Receive input", 5000, (void *)&char_buf, 10, NULL, app_cpu);
        
        // Function waits indefinitely and is blocked from returning until this succeeds
        xSemaphoreTake(bin_sem, portMAX_DELAY);
}