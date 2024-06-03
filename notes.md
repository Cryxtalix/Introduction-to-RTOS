# Introduction to RTOS

## Part 1 - What is a Real Time Operating System(RTOS)?

General purpose OS like windows or linux are designed with human interaction as the most important feature, and the scheduler prioritises such tasks. Scheduler is non-deterministic and we can't tell exactly which tasks will be performed when and for how long. Not ideal when strict timing dealines are required, like medical or industrial devices.

RTOS has many of the same functions as a general purpose OS, but the it is designed such that the scheduler can guarantee meeting timing deadlines on the task.

Common superloop program flow is easy to use, but doesn't allow for concurrent tasks.

* Task - Set of program instructions loaded in memory.
* Thread - Unit of CPU utilization with its own program counter and stack.
* Process - Instance of a computer program, might contain serveral threads.
FreeRTOS: Task = Thread

An RTOS might take up too much memory and processing resources of a microcontroller, but is viable running on a more powerful device like an ESP32. An ESP32 is capable of running concurrent tasks, even the wireless stack that takes up a lot of ram and processing power. Running and RTOS on an ESP32 allows it to meet strict timing deadlines.

## Part 2 - Getting started with FreeRTOS

ESPIDF runs a modified version of FreeRTOS to support the ESP32's symmetric multiprocessing architecture that utilises the multiple cores. [Here is a list of changes](https://docs.espressif.com/projects/esp-idf/en/release-v4.3/esp32/api-guides/freertos-smp.html) made to the [IDF FreeRTOS](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos_idf.html).

### Code
* `vTaskDelay(500 / portTICK_PERIOD_MS)` 500ms non-blocking delay for FreeRTOS
* `vTaskStartScheduler()` Only in vanilla FreeRTOS, to start the task

Create task:

    xTaskCreatePinnedToCore( // Creates task with affinity to specified CPU core
    	myFunc, // Function to be called
    	"Special function", // Name of task
    	1024, // Stack size (bytes in ESPIDF, words in FreeRTOS)
    	NULL, // Parameter to pass to function
    	1, // Task priority (0 to configMAX_PRIORITIES-1)
    	&func_handle, // Task handle
    	app_cpu // CPU core (static const BaseType_t app_cpu = 1)
    );

* `static const char *TAG = "Module_A";` Create log tag
* `ESP_LOGW(tag_a, "Invalid input");` Log warning
* Also ESP_LOGE(error), ESP_LOGI(info), ESP_LOGD(debug), ESP_LOGV(verbose)

## Part 3 - Task Scheduling
States where tasks can be in:

* Ready: Can run at any time, but not yet given CPU time by the scheduler
* Running: Chosen by the scheduler to run
* Blocked: Blocking API called, such as waiting for delay, queue or semaphore
* Suspended: Explicitly suspended with vTaskSuspend(). Can only be resumed by vTaskResume().

For a task, all the current values in ram and CPU registers during it's execution such as current program instructions and working memory is known as the context. When the scheduler chooses a different task to run, the context will be saved, while the incoming task's context will be loaded so that it can carry on where it left off earlier. The process of saving and restoring task execution context is known as context switching. This enables multiple tasks to run on a single CPU core.

Much of this context is stored in the task stack, and therefore every task created has to have a minimum amount of stack memory allocated.

### Code
* `vTaskSuspend()` Enters the suspended state  
* `vTaskResume()` Enters the ready state  

## Part 4 - Memory Management

* **Static memory** is determined at compile time, and stores **global and static variables**. This is known as **static allocation** and the variables here exist for the duration of the program.

* **Stack memory** stores functions and local variables, which also includes arguments and return addresses. Stack is LIFO system, variables are continually pushed to the stack when calling functions, and popped off the stack upon returning from a function. The stack typically grows towards the heap.

* **Heap memory** is used for dynamic allocation, where the programmer explicitly reserves memory through functions like malloc. Allocated memory has to be freed after use, or the heap might grow indefinitely, known as a memory leak. As it grows towards the stack, it might even overwrite areas of the stack causing undefined behaviour.

### ESP32
In ESP32, the heap contains dynamically allocated memory, tasks and kernel objects. 

A task created with xTaskCreate() is assigned a portion of memory from the heap. The portion is divided up into a task control block(TCB) and stack for the task. The TCB is a stuct which contains information about the task like task stack address and priority level. The stack's size can be changed from xTaskCreate().

Kernel objects include queues and semaphores.

During dynamic allocation, the largest contiguous block of heap memory available is usually allocated. With repeated allocation and freeing of this heap memory, it might get fragmented, resulting in it growing faster than expected. Thus, FreeRTOS contains several selectable heap allocation schemes to choose from.

In newer versions of FreeRTOS, you can use xTaskCreateStatic() to allocate static memory for tasks and kernel objects in static memory. This is useful for mission critical applications where memory leaks cannot be allowed to occur.

### Code
* `pvPortMalloc()` Thread safe version of malloc for FreeRTOS  
* `vPortFree(ptr)` Thread safe version of free for FreeRTOS  
* `uxTaskGetStackHighWaterMark(NULL)` Get remaining stack memory in words  
* `xPortGetFreeHeapSize()` Get remaining heap memory in bytes  

## Part 5 - Queues
The data bus width of the ESP32 is 32 bits wide, this is the number of bits that it can process, transfer or access in a single operation. When working with data greater than 32 bits, such as storing a 64 bit number, the operation could take multiple cycles to complete. There is a chance that the operations do not occur consecutively, and another operation could pre-empt the second operation. If the other operation is also accessing the same memory location, it could result in data corruption or race conditions and other unpredictable results.

To solve this issue, we need atomic operations that guarantee the operations are completed without interruption.

A queue is a FIFO data structure provided by FreeRTOS to pass data between tasks. As long as we interact with the queue with built-in kernel functions, these operations are guaranteed to be atomic. Another task cannot interupt the writing process. 

Interupt service routines do not depend on the tick timer and will not respect the atomic nature of the processes, so we need to use special FromISR functions.

Unless explictly made to store a pointer, queues are copy-by-reference, and will store an full copy of the data. We need to ensure any pointers point to memory that is still in-scope when we read from it.

https://www.freertos.org/a00018.html

### Code
* `static QueueHandle_t my_queue;` Create queue handle
* `my_queue = xQueueCreate(queue_len, sizeof(data));` Create queue
	* `my_queue == NULL` if unsuccessful
* `xQueueSend(my_queue, (void *)&num, 10)` Send data to queue, returns pdTRUE on success  
* `xQueueReceive(my_queue, (void *)&item, 10)` Receive data from queue, returns pdTRUE on success 
	* 10 is number of ticks to wait if data does get sent or received, such as full or empty queue
	* Alternatively `portMAX_DELAY` to wait indefinitely

## Part 6 - Mutex
A race condition is an issue where a system's behaviour is dependent on the timing on uncontrollable events.The section of code that works with a shared global resources is known as a critical section. A critical section works with shared resources and must execute in its entirety by a task before it can be interupted by another task. Mutual exclusion protects critical sections by preventing other tasks from interupting it before it is has completed its execution entirely. There are several kernel objects to choose from to accomplish the task.

A mutex at it's simplest form, is a boolean variable conveying whether a shared resource is in use or not. Shared resources should not be accessed if the mutex is not available.

### Protecting shared resources and synchronizing threads
* Queue: Pass data between threads
* Lock: Allows only one thread to enter the "locked" section of code
* Mutex(MUTual EXclusion): Like a lock, but system wide and shared by multiple processes
* Semaphore: Allows a set number of threads to enter a critical section, used to signal to other threads

Interupt service routines do not depend on the tick timer and will not respect the atomic nature of the processes, so we need to use special FromISR functions.

https://www.freertos.org/a00113.html

### Code
* `#include "freertos/semphr.h"` For vanilla FreeRTOS
* `static SemaphoreHandle_t my_mutex;` Create mutex handle, mutex and semaphore handles are handled together in FreeRTOS
* `my_mutex = xSemaphoreCreateMutex();` Create mutex
	* `my_mutex == NULL` if unsuccessful
* `xSemaphoreTake(my_mutex, 10)` Take semaphore, returns pdTRUE on success. Use this as conditional for an if statement to protect a critical section.
	* 10 is number of ticks to wait
	* Alternatively `portMAX_DELAY` to wait indefinitely
* `xSemaphoreGive(my_mutex)` Return mutex

## Part 7 - Semaphore
Semaphore is like a mutex, but a positive number rather than just a boolean. A semaphore has an initial value >1, and can be taken multiple times until it reaches 0. However it is not usually used in the same way as mutexes.

Semaphores are typically used to synchronise threads, such as in a producer/consumer model. One or more tasks add or remove data from a shared resources that can grow unbounded, such as a linked list or buffer. We can limit the maximum size of the resource by setting the maximum size of the semaphore. Note that a mutex might still be necessary to protect the reading and writing process to the shared resource.

* Mutex
	* Imply ownership of a given resource
	* Many implementations of mutexes such as in FreeRTOS include a form of priority inheritance, the priority level of tasks that hold mutexes are automatically raised to prevent a higher priority task from being blocked for a long time. 

* Semaphore
	* Can be used from ISR.

### Code
* `static SemaphoreHandle_t my_sem;` Create semaphore handle
* `my_sem = xSemaphoreCreateBinary();` Creates binary semaphore(only goes up to 1)
	* `my_sem == NULL` if unsuccessful
* `my_sem = xSemaphoreCreateCounting(total_num, initial_num);` Creates counting semaphore
	* `my_sem == NULL` if unsuccessful
* `xSemaphoreTake(my_sem, 10)` Take semaphore, returns pdTRUE on success. Use this as conditional for an if statement to protect a critical section.
	* 10 is number of ticks to wait
	* Alternatively `portMAX_DELAY` to wait indefinitely
* `xSemaphoreGive(my_sem)` Return mutex

## Part 8 - Software Timer
### Possible approaches to perform a task periodically
* `vTaskDelay()` cannot be used to define regular intervals if the task itself takes time to complete.
* `xTaskGetTickCount()` will require setting up a whole task to keep time, and will compete for resources.
* Hardware timers are limited and hardware dependent, making our code non-portable.
* Software timers are a good option

We can use the FreeRTOS timer library. The timer service task(timer daemon) will be created in the background that maintains a list of timers and calling the associated callback functions when any of them expires. It can block itself and wake up when the timers expire, therefore not competing for resources with other tasks. The timer service task also persists beyond the app_main returning.

Since the timer service task calls the callback function, the function has the same priority level as the timer service task itself. We also want to treat these callback functions as if they were ISRs, therefore we generally do not want to use delay functions or allow for blocking when using queues, mutexes and semaphores inside the callback functions. We do not want to control the timer service task directly.

FreeRTOS provides a timer command queue and API functions to access it. To send commands to the timer service task, we use the API functions to send the commands to the timer command queue. The timer service task will read the timer command queue and perform the commands like creating a new timer, start, stop, restart etc.

https://www.freertos.org/FreeRTOS-Software-Timer-API-Functions.html

### Code
* `static TimerHandle_t my_timer = NULL;` Create timer handle

		// Create timer
        my_timer = xTimerCreate (
                "My timer",               // Name
                2000 / portTICK_PERIOD_MS,      // Period of timer (ticks)
                pdFALSE,                        // Auto-reload
                (void *)0,                      // Timer ID (uint32, but must cast to void pointer)
                timer_callback                  // Callback function
        );

* `xTimerStart(one_shot_timer, portMAX_DELAY);` Start timer

## ESP32-S3 board
### Serial Terminal

Successful setup would allow using standard C functions like printf and fgets. 

On ESP-IDF, open `idf.py menuconfig`. Navigate to `Component Config/ESP System Settings/Channel for console output` option and ensure that `USB Serial/JTAG Controller` is selected.

On forums, it was recommended to install the usb-serial-jtag driver, but that is no longer necessary.

Stdout buffer is on by default, so output only gets printed when a newline is added. To print any char, use fsync(fileno(stdout)) to flush the stdout.

https://github.com/espressif/esp-idf/issues/2820

### GPIO Pins

* When starting, we want to reset the pin we want to use with `gpio_reset_pin(PIN_NUM)`. Then set the pin's direction(1 for on or off) with `gpio_set_direction(PIN_NUM, GPIO_MODE_OUTPUT);`.
* Set pins with `gpio_set_level(PIN_NUM, 1);`.