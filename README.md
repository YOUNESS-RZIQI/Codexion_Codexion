*This project has been created as part of the 42 curriculum by yrziqi.*

# Multithreading in C And Priority Queue Min-Heap

<p align="center">
  <img src="./Codexion.png" alt="Codexion: The Concurrency Challenge" width="100%">
</p>

## Description
Codexion is a concurrency challenge simulating a collaborative coding environment. In this simulation, coders sit in a circular workspace around a shared Quantum Computer and must take turns compiling, debugging, and refactoring. The primary challenge arises from the limited availability of USB dongles—each coder requires two dongles (one for each hand) to compile their quantum code, yet there are only as many dongles as there are coders. 

The goal of this project is to implement a highly robust, multi-threaded C program that orchestrates these coders while preventing resource exhaustion, deadlocks, and thread starvation. Through careful synchronization using POSIX threads, mutexes, and scheduling algorithms (FIFO and Earliest Deadline First), the system must guarantee that every coder gets fair access to the shared dongles without "burning out" due to long wait times.

## Instructions
### Compilation
The project uses a standard Makefile to compile the executable `codexion`.
To compile the project with the required flags (`-Wall -Wextra -Werror -pthread`), simply run:
```bash
make
```
Other available rules:
- `make clean`: Removes object files.
- `make fclean`: Removes object files and the executable.
- `make re`: Fully recompiles the project.

### Execution
Run the program with the following mandatory arguments:
```bash
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

#### Arguments Description:
- `number_of_coders`: Number of coders (and dongles).
- `time_to_burnout`: Milliseconds a coder can wait without starting to compile before burning out.
- `time_to_compile`: Milliseconds required to compile.
- `time_to_debug`: Milliseconds spent debugging.
- `time_to_refactor`: Milliseconds spent refactoring.
- `number_of_compiles_required`: Target number of compiles for each coder to successfully end the simulation.
- `dongle_cooldown`: Milliseconds a dongle must rest before it can be used again.
- `scheduler`: Arbitration policy (`fifo` or `edf`).

#### Example:
```bash
./codexion 5 340 100 100 100 5 10 edf
```
```bash
./codexion 199 340 100 100 100 5 10 fifo
```
## Blocking Cases Handled
Our implementation robustly handles critical concurrency edge cases:
- **Deadlock Prevention & Coffman’s Conditions:** By breaking one of the Coffman Conditions—Mutual Exclusion, Hold and Wait, No Preemption, or Circular Wait—we can prevent deadlocks. And in this project, we prevented deadlocks by breaking the Circular Wait condition.
- **Starvation Prevention:** We guarantee liveness by strictly honoring our scheduling queues. In FIFO, requests are served in exact arrival order. In EDF (Earliest Deadline First), dongles are granted to the coder closest to burning out, ensuring no single coder is left behind.
## Thread Synchronization Mechanisms
This project relies heavily on POSIX thread synchronization mechanisms to manage resource distribution securely:
- **Mutexes (`pthread_mutex_t`):** 
  - Each dongle state is safeguarded by its own mutex to prevent concurrent acquisitions. 
  - A global lock protects the print statements to prevent garbled output.
  - The monitor thread utilizes mutexes to read coder last-compile timestamps safely, avoiding race conditions during state transitions.
- **Condition Variables (`pthread_cond_t`):** 
  - Condition variables efficiently manage the waiting queues for dongles. Instead of spin-locking and consuming excessive CPU cycles, threads wait on condition variables until a dongle becomes available and the scheduler assigns it to them.
- **Queue and Synchronization Logic:**
  - When coders require dongles, they enter an arbitration phase by pushing their request to a custom min-heap structure (representing our FIFO/EDF scheduler) and wait. When a coder finishes compiling, they release the dongles, signaling the condition variable to wake up the arbitration system, which then pops the highest-priority request and grants the dongle.

## Resources
- **Documentation:**
- **Mutex (`pthread_mutex_init()`):** 
  - [Mutex Initializer](https://linux.die.net/man/3/pthread_mutex_init)
- **Cond (`pthread_cond_init()`):** 
  - [Condition Variables Initializer](https://linux.die.net/man/3/pthread_cond_init)
- **Mutexes (`pthread_mutex_lock()` , `pthread_mutex_unlock()`):** 
  - [Mutexe lock & unlock](https://linux.die.net/man/3/pthread_mutex_lock)
- **Cond (`pthread_cond_wait()` , `pthread_cond_timedwait()`):** 
  - [Condition Variables cond_wait & timedwait](https://linux.die.net/man/3/pthread_cond_wait)

- **Or you can just check in the Terminal:** 
  - man pthread_create
  - man pthread_join
  - man pthread_mutex_lock
  - man pthread_mutex_init
  - man pthread_mutex_destroy
  - ...
- **Concepts:**
  - [Coffman's conditions for deadlocks.](https://www.lenovo.com/us/en/glossary/what-is-deadlock/?orgRef=https%253A%252F%252Fwww.google.com%252F)
  - [Priority Queues & Min-Heaps for implementing the FIFO and EDF scheduling logic..](https://www.geeksforgeeks.org/dsa/priority-queue-set-1-introduction/)
- **AI Usage:**
  - AI tools were utilized as study aids to understand nuanced POSIX behaviors, deadlock prevention, and complex edge cases that are notoriously difficult to debug using GDB in multithreaded environments.
   For ex : 
    * Why we do have the while in the `pthread_cond_wait()` , because of the `spurious wake ups`.
