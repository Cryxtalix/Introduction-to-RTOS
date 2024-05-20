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

## Code
`vTaskDelay(500 / portTICK_PERIOD_MS)` 500ms non-blocking delay for FreeRTOS  

    xTaskCreatePinnedToCore( // Creates task with affinity to specified CPU core
    	myFunc, // Function to be called
    	"Special function", // Name of task
    	1024, // Stack size (bytes in ESPIDF, words in FreeRTOS)
    	NULL, // Parameter to pass to function
    	1, // Task priority (0 to configMAX_PRIORITIES-1)
    	&func_handle, // Task handle
    	app_cpu // CPU core (static const BaseType_t app_cpu = 1)
    );

`vTaskStartScheduler()` Only in vanilla FreeRTOS, to start the task  

# Part 3 - Task Scheduling
States where tasks can be in:

* Ready: Can run at any time, but not yet given CPU time by the scheduler
* Running: Chosen by the scheduler to run
* Blocked: Blocking API called, such as waiting for delay, queue or semaphore
* Suspended: Explicitly suspended with vTaskSuspend(). Can only be resumed by vTaskResume().

For a task, all the current values in ram and CPU registers during it's execution such as current program instructions and working memory is known as the context. When the scheduler chooses a different task to run, the context will be saved, while the incoming task's context will be loaded so that it can carry on where it left off earlier. The process of saving and restoring task execution context is known as context switching. This enables multiple tasks to run on a single CPU core.

Much of this context is stored in the task stack, and therefore every task created has to have a minimum amount of stack memory allocated.

## Code
`vTaskSuspend()` Enters the suspended state  
`vTaskResume()` Enters the ready state  

# Part 4 - Memory Management

* **Static memory** is determined at compile time, and stores **global and static variables**. This is known as **static allocation** and the variables here exist for the duration of the program.

* **Stack memory** stores functions and local variables, which also includes arguments and return addresses. Stack is LIFO system, variables are continually pushed to the stack when calling functions, and popped off the stack upon returning from a function. The stack typically grows towards the heap.

* **Heap memory** is used for dynamic allocation, where the programmer explicitly reserves memory through functions like malloc. Allocated memory has to be freed after use, or the heap might grow indefinitely, known as a memory leak. As it grows towards the stack, it might even overwrite areas of the stack causing undefined behaviour.

## ESP32
In ESP32, the heap contains dynamically allocated memory, tasks and kernel objects. 

A task created with xTaskCreate() is assigned a portion of memory from the heap. The portion is divided up into a task control block(TCB) and stack for the task. The TCB is a stuct which contains information about the task like task stack address and priority level. The stack's size can be changed from xTaskCreate().

Kernel objects include queues and semaphores.

During dynamic allocation, the largest contiguous block of heap memory available is usually allocated. With repeated allocation and freeing of this heap memory, it might get fragmented, resulting in it growing faster than expected. Thus, FreeRTOS contains several selectable heap allocation schemes to choose from.

In newer versions of FreeRTOS, you can use xTaskCreateStatic() to allocate static memory for tasks and kernel objects in static memory. This is useful for mission critical applications where memory leaks cannot be allowed to occur.

## Code
`pvPortMalloc()` Thread safe version of malloc for FreeRTOS
`vPortFree(ptr)` Thread safe version of free for FreeRTOS
`uxTaskGetStackHighWaterMark(NULL)` Get remaining stack memory in words
`xPortGetFreeHeapSize()` Get remaining heap memory in bytes

# Serial communication on ESP32-S3

Successful setup would allow using standard C functions like printf and fgets. 

On ESP-IDF, open `idf.py menuconfig`. Navigate to `Component Config/ESP System Settings/Channel for console output` option and ensure that `USB Serial/JTAG Controller` is selected.

On forums, it is recommended to install the usb-serial-jtag driver, but that is no longer necessary.