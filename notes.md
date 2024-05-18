# Part 1 - What is a Real Time Operating System(RTOS)?

General purpose OS like windows or linux are designed with human interaction as the most important feature, and the scheduler prioritises such tasks. Scheduler is non-deterministic and we can't tell exactly which tasks will be performed when and for how long. Not ideal when strict timing dealines are required, like medical or industrial devices.

RTOS has many of the same functions as a general purpose OS, but the it is designed such that the scheduler can guarantee meeting timing deadlines on the task.

Common superloop program flow is easy to use, but doesn't allow for concurrent tasks.

* Task - Set of program instructions loaded in memory.
* Thread - Unit of CPU utilization with its own program counter and stack.
* Process - Instance of a computer program, might contain serveral threads.
FreeRTOS: Task = Thread

An RTOS might take up too much memory and processing resources of a microcontroller, but is viable running on a more powerful device like an ESP32. An ESP32 is capable of running concurrent tasks, even the wireless stack that takes up a lot of ram and processing power. Running and RTOS on an ESP32 allows it to meet strict timing deadlines.

# Part 2 - Getting started with FreeRTOS

ESPIDF runs a modified version of FreeRTOS to support the ESP32's symmetric multiprocessing architecture that utilises the multiple cores. [Here is a list of changes](https://docs.espressif.com/projects/esp-idf/en/release-v4.3/esp32/api-guides/freertos-smp.html) made to the [IDF FreeRTOS](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos_idf.html).

# Serial communication on ESP32-S3

Successful setup would allow using standard C functions like printf and fgets. 

On ESP-IDF, open `idf.py menuconfig`. Navigate to `Component Config/ESP System Settings/Channel for console output` option and ensure that `USB Serial/JTAG Controller` is selected.

On forums, it is recommended to install the usb-serial-jtag driver, but that is no longer necessary.