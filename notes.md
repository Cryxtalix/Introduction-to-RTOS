# Part 1 - What is a Real Time Operating System(RTOS)?

General purpose OS like windows or linux are designed with human interaction as the most important feature, and the scheduler prioritises such tasks. Scheduler is non-deterministic and we can't tell exactly which tasks will be performed when and for how long. Not ideal when strict timing dealines are required, like medical or industrial devices.

RTOS has many of the same functions as a general purpose OS, but the it is designed such that the scheduler can guarantee meeting timing deadlines on the task.

Common superloop program flow is easy to use, but doesn't allow for concurrent tasks.

* Task - Set of program instructions loaded in memory.
* Thread - Unit of CPU utilization with its own program counter and stack.
* Process - Instance of a computer program