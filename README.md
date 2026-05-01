*This project has been created as part of the 42 curriculum by yrziqi.*

# Codexion: Master the race for resources before the deadline masters you

## Description
Codexion is a concurrency challenge simulating a collaborative coding environment. In this simulation, coders sit in a circular workspace around a shared Quantum Compiler and must take turns compiling, debugging, and refactoring. The primary challenge arises from the limited availability of USB dongles—each coder requires two dongles (one for each hand) to compile their quantum code, yet there are only as many dongles as there are coders. 

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
./codexion 5 800 200 200 200 5 10 fifo
```

## Blocking Cases Handled
Our implementation robustly handles critical concurrency edge cases:
- **Deadlock Prevention & Coffman’s Conditions:** Deadlocks are prevented by ensuring dongles are acquired through a central arbitration mechanism rather than a circular wait system. Requests are routed through our scheduling queues, avoiding scenarios where adjacent coders hold one dongle and wait indefinitely for a second.
- **Starvation Prevention:** We guarantee liveness by strictly honoring our scheduling queues. In FIFO, requests are served in exact arrival order. In EDF (Earliest Deadline First), dongles are granted to the coder closest to burning out, ensuring no single coder is left behind.
- **Cooldown Handling:** Dongle availability states are tied to timestamp records, ensuring that when a dongle is released, it is not re-granted until the specified `dongle_cooldown` duration has fully elapsed.
- **Precise Burnout Detection:** A dedicated monitor thread polls the state of all coders continuously. It is decoupled from the coder logic to strictly enforce burnout detection within the requested 10 ms tolerance.
- **Log Serialization:** All console output is routed through a dedicated mutex. This guarantees that logs from different threads never interleave or write concurrently to standard output.

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
  - Thread-safe communication between the coders and the monitor is achieved by locking specific attributes (like `stop_simulation` flag) before reading or modifying, guaranteeing that the monitor can precisely detect burnouts without reading partially updated states.

## Resources
- **Documentation:**
  - man of pthread_create , pthread_join , pthread_mutex_lock ...
  - [Mutexes and Condition Variables](https://cs341.cs.illinois.edu/coursebook/Deadlock)
- **Concepts:**
  - Coffman's conditions for deadlocks.
  - Priority Queues & Min-Heaps for implementing the FIFO and EDF scheduling logic.
- **AI Usage:**
  - AI tools were utilized primarily as study aids to understand nuanced POSIX behaviors, to help clarify the nuances between `pthread_cond_signal` and `pthread_cond_broadcast`, and to review edge cases in our EDF min-heap implementation. No AI-generated code was blindly incorporated; all patterns were thoroughly analyzed, verified for correctness, tested locally, and adapted to adhere strictly to the 42 Norms.
