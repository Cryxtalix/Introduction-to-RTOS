// Create two tasks to control blinking LED. One task listens for 
// input from the serial terminal, while the other blinks the LED.

#include "tdisplays3.h"

int blink_interval = 1000;

void listener(void *params)
{
        char *serial_input = malloc(32 * sizeof(char));
        memset(serial_input, '\0', 32);
        int buffer;

  	while(1) {
                buffer = fgetc(stdin);
                // Check valid input, ignore anything that isn't a number or newline
	        if ((buffer != EOF && buffer > 47 && buffer < 58) || (char)buffer == '\n') {
                        if ((char)buffer == '\n') {
                                if (strlen(serial_input) > 0) {
                                        blink_interval = atoi(serial_input);
                                        printf("Blink interval set to: %d\n", blink_interval);
                                        memset(serial_input, '\0', 32);
                                }
                        } else {
                                serial_input[strlen(serial_input)] = (char)buffer;
                        }
                }
                vTaskDelay(10 / portTICK_PERIOD_MS);
        }
}

void blink_led(void *params)
{
        while(1) {
                gpio_set_level(PIN_LCD_BL, 1);
                vTaskDelay(blink_interval / portTICK_PERIOD_MS);
                gpio_set_level(PIN_LCD_BL, 0);
                vTaskDelay(blink_interval / portTICK_PERIOD_MS);
        }
}

void app_main(void)
{
        char *ourTaskName = pcTaskGetName(NULL);
        ESP_LOGI(ourTaskName, "Hello, starting up!\n");

        gpio_reset_pin(PIN_LCD_BL);
        gpio_set_direction(PIN_LCD_BL, GPIO_MODE_OUTPUT);

        xTaskCreatePinnedToCore( // Specific to esp32 freertos
                listener,       // Function to be called
                "listen for input from serial",    // Name of Task
                10000,            // Stack size (bytes in esp32, words in freertos)
                NULL,            // Parameter to pass to function
                1,               // Task priority (0 to configMAX_PRIORITIES - 1)
                NULL,            // Task handle 
                app_cpu          // Run in core 1 only
        );

        xTaskCreatePinnedToCore( // Specific to esp32 freertos
                blink_led,       // Function to be called
                "blinks led",    // Name of Task
                2048,            // Stack size (bytes in esp32, words in freertos)
                NULL,            // Parameter to pass to function
                2,               // Task priority (0 to configMAX_PRIORITIES - 1)
                NULL,            // Task handle 
                app_cpu          // Run in core 1 only
        );

        // If this was vanilla freertos, we would want to call vTaskStartScheduler() in main
        // after setting up the tasks. ESP-IDF setup function calls it automatically.
}