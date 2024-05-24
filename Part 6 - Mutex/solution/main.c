#include "tdisplays3.h"

static SemaphoreHandle_t mutex;

/**
 * @brief Receives the address of a variable passed from app_main
 * 
 * @param params 
 */
static void receive(void *params)
{
        if (xSemaphoreTake(mutex, 10) == pdTRUE) {
                char my_char = *(char *)params;
                printf("Task received: %c\n", my_char);
                xSemaphoreGive(mutex);
                vTaskDelete(NULL);
        }
}

/**
 * @brief Passing a variable from stack memory to a task by using a mutex to ensure the lifetime of the parameter.
 * 
 */
void app_main(void)
{
        mutex = xSemaphoreCreateMutex();

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
        
        while(1) {
                vTaskDelay(10 / portTICK_PERIOD_MS);
                if(uxSemaphoreGetCount(mutex) == 1) {
                        break;
                }
        }
}