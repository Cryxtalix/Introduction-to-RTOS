#include "tdisplays3.h"

char *serial_input;
bool ready = 0;

/**
 * @brief Listen for input from serial monitor, store all chars upon receiving newline to heap. Notify task b.
 * 
 * @param params 
 */
void task_A(void *params)
{
        char str_buf[32] = {'\0'};
        int char_buf;

  	while(1) {
                char_buf = fgetc(stdin);
                if (char_buf != EOF) {
                        str_buf[strlen(str_buf)] = (char)char_buf;
                        if ((char)char_buf == '\n') {
                                serial_input = malloc(32 * sizeof(char));
                                strcpy(serial_input, str_buf);
                                memset(str_buf, '\0', 32);
                                ready = 1;
                        }
                }
                vTaskDelay(10 / portTICK_PERIOD_MS);
        }
}

/**
 * @brief Waits for task A, prints message from heap, free heap.
 * 
 * @param params 
 */
void task_B(void *params)
{
        while(1) {
                if (ready == 1) {
                        printf("%s", serial_input);
                        free(serial_input);
                        ready = 0;
                }
                vTaskDelay(10 / portTICK_PERIOD_MS);
        }
}

void app_main(void)
{
        char *ourTaskName = pcTaskGetName(NULL);
        ESP_LOGI(ourTaskName, "Hello, starting up!\n");

        gpio_reset_pin(PIN_LCD_BL);
        gpio_set_direction(PIN_LCD_BL, GPIO_MODE_OUTPUT);

        xTaskCreatePinnedToCore( // Specific to esp32 freertos
                task_A,       // Function to be called
                "listen for input from serial",    // Name of Task
                10000,            // Stack size (bytes in esp32, words in freertos)
                NULL,            // Parameter to pass to function
                1,               // Task priority (0 to configMAX_PRIORITIES - 1)
                NULL,            // Task handle 
                app_cpu          // Run in core 1 only
        );

        xTaskCreatePinnedToCore( // Specific to esp32 freertos
                task_B,       // Function to be called
                "prints string",    // Name of Task
                10000,            // Stack size (bytes in esp32, words in freertos)
                NULL,            // Parameter to pass to function
                2,               // Task priority (0 to configMAX_PRIORITIES - 1)
                NULL,            // Task handle 
                app_cpu          // Run in core 1 only
        );
}