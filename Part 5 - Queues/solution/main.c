#include "tdisplays3.h"

static const char *tag_A = "task_A";
static const char *tag_B = "task_B";

static QueueHandle_t queue_1;
static QueueHandle_t queue_2;

typedef struct {
        int times_blinked;
        char msg[10];
} blink_msg;

static void task_A(void *params)
{
        // Strings from serial
        int char_buf;
        char str_buf[32] = {'\0'};

        // Detected number
        int delay;

        // Receive from queue
        blink_msg queue_buf;

        while(1) {
                // Read serial input
                char_buf = fgetc(stdin);

                if (char_buf != EOF) {
                        if ((char)char_buf == '\n') {
                                printf("%s\n", str_buf);

                                if (strncmp(str_buf, "delay ", 6) == 0) {
                                        char tmp[26];
                                        strncpy(tmp, str_buf + 6, strlen(str_buf) - 5);
                                        delay = atoi(tmp);
                                        if (delay != 0) {
                                                if (xQueueSend(queue_1, &delay, 0) != pdTRUE) {
                                                        ESP_LOGW(tag_A, "Error sending to queue 1");
                                                }
                                        }
                                }
                                memset(str_buf, '\0', 32);
                        } else {
                                str_buf[strlen(str_buf)] = (char)char_buf;
                        }
                }

                // Read and print from queue 2
                if (xQueueReceive(queue_2, &queue_buf, 0) == pdTRUE) {
                        printf("%s. Total times blinked: %d.\n", queue_buf.msg, queue_buf.times_blinked);
                }

                vTaskDelay(10 / portTICK_PERIOD_MS);
        }
}

static void task_B(void *params)
{
        bool led = 0;
        int t = 1000; // blink delay
        blink_msg my_msg;
        my_msg.times_blinked = 0;
        strcpy(my_msg.msg, "Blinked.");

        while(1) {
                //Update t
                if (uxQueueMessagesWaiting(queue_1) > 0) {
                        if (xQueueReceive(queue_1, &t, 0) != pdTRUE) {
                                ESP_LOGW(tag_B, "Error reading from queue 1");
                        }
                }

                // Upon 100 blinks
                // Check led == 1, or will match on the next loop too
                if (led == 1 && my_msg.times_blinked % 100 == 0 && my_msg.times_blinked > 0) {
                        if (xQueueSend(queue_2, &my_msg, 0) != pdTRUE) {
                                ESP_LOGW(tag_B, "Error sending to queue 2");
                        }
                }

                // Blink led
                gpio_set_level(PIN_LCD_BL, led);
                led = !led;
                if (led == 1) {
                        my_msg.times_blinked += 1;
                }

                vTaskDelay(t / portTICK_PERIOD_MS);
        }
}

void app_main(void)
{
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        gpio_reset_pin(PIN_LCD_BL);
        gpio_set_direction(PIN_LCD_BL, GPIO_MODE_OUTPUT);

        uint8_t queue_len = 5;

        queue_1 = xQueueCreate(queue_len, sizeof(int));
        queue_2 = xQueueCreate(queue_len, sizeof(blink_msg));

        xTaskCreatePinnedToCore(task_A, "Read input", 10000, NULL, 1, NULL, app_cpu);
        xTaskCreatePinnedToCore(task_B, "Blink LED", 10000, NULL, 1, NULL, app_cpu);
}