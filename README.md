*This project has been created as part of the 42 curriculum by <login>.*

---

# Codexion 🔌

> **Master the race for resources before the deadline masters you.**

---

## Description

**Codexion** is a concurrency simulation written in C using POSIX threads.  
It models a group of **coders** sitting around a circular table, each needing **two USB dongles** simultaneously to compile their quantum code. Since dongles are shared between neighbours, coders must coordinate carefully to avoid **deadlocks**, **starvation**, and **burnout**.

The simulation stops either when a coder burns out (fails to compile in time) or when every coder has compiled a required number of times.

**Key concepts practiced:**
- POSIX threads (`pthread_create`, `pthread_join`)
- Mutual exclusion (`pthread_mutex_t`)
- Condition variables (`pthread_cond_t`, `pthread_cond_wait`, `pthread_cond_broadcast`)
- Scheduling algorithms: **FIFO** and **EDF** (Earliest Deadline First)
- Min-heap priority queue implemented from scratch
- Precise burnout detection within a 10 ms window

---

## Instructions

### Compilation

```bash
# Clone the repository
git clone <repo_url>
cd codexion

# Build
make

# Clean object files
make clean

# Full clean (removes binary too)
make fclean

# Rebuild from scratch
make re
```

### Execution

```bash
./codexion number_of_coders time_to_burnout time_to_compile \
           time_to_debug time_to_refactor number_of_compiles_required \
           dongle_cooldown scheduler
```

### Arguments

| Argument | Description |
|---|---|
| `number_of_coders` | Number of coders (and dongles). Must be > 0 |
| `time_to_burnout` | Milliseconds before a coder burns out without compiling |
| `time_to_compile` | Milliseconds to compile (must hold both dongles) |
| `time_to_debug` | Milliseconds spent debugging |
| `time_to_refactor` | Milliseconds spent refactoring |
| `number_of_compiles_required` | Simulation stops when all coders reach this count |
| `dongle_cooldown` | Milliseconds a dongle rests after being released |
| `scheduler` | `fifo` or `edf` |

### Example runs

```bash
# 5 coders, generous timing, FIFO
./codexion 5 800 200 200 200 5 0 fifo

# 4 coders, EDF scheduler, 50ms cooldown
./codexion 4 1200 200 200 200 3 50 edf
```

### Expected log format

```
0 1 has taken a dongle
1 1 has taken a dongle
1 1 is compiling
201 1 is debugging
401 1 is refactoring
402 2 has taken a dongle
403 2 has taken a dongle
403 2 is compiling
603 2 is debugging
```

---

## Blocking cases handled

### Deadlock prevention (Coffman's conditions)
The classic dining philosophers deadlock arises when every coder holds one dongle and waits for the other in a circle — **circular wait** (Coffman condition #4).

**Solution:** A **resource ordering** strategy is applied. Every coder always acquires the **lower-indexed dongle first**, then the higher-indexed one. The last coder (whose right dongle wraps to index 0) therefore takes dongle `0` before dongle `N-1`, breaking the cycle.

This eliminates circular wait without requiring a central arbiter.

### Starvation prevention
Under FIFO, requests are served strictly in arrival order — no coder is ever skipped.  
Under EDF, the coder with the earliest burnout deadline is served first, which is inherently starvation-resistant because a coder who has not compiled recently will have the closest deadline and therefore the highest priority.

### Cooldown handling
After a coder releases a dongle, `dongle_cooldown` milliseconds must pass before anyone can take it. The dongle's `cooldown_until` timestamp is set on release. In the `take_dongle` wait loop, a coder only breaks out if `now_ms() >= d->cooldown_until` in addition to the dongle being available and it being their turn in the queue.

### Precise burnout detection
A dedicated **monitor thread** polls every **1 millisecond**. It checks whether `now_ms() > last_compile_start + time_to_burnout` for each coder. Since polling happens every 1 ms, burnout is logged within at most 1 ms of the actual deadline — well inside the required 10 ms window.

### Log serialization
All output goes through a single `log_event()` function which acquires `sim->log_mutex` before every `printf`. This guarantees that two messages never interleave on the same line, even with many concurrent threads.

---

## Thread synchronization mechanisms

### `pthread_mutex_t` — mutual exclusion locks

| Mutex | Protects |
|---|---|
| `sim->log_mutex` | All `printf` / log output — prevents interleaved messages |
| `sim->monitor_mutex` | The `sim->stop` flag — ensures visibility between threads |
| `dongles[i].mutex` | All fields of each dongle: `available`, `cooldown_until`, `queue` |

**Race condition example prevented:**  
Without `dongles[i].mutex`, two coder threads could both read `available == 1` and both proceed to mark it `0` — two coders would hold the same dongle simultaneously.

### `pthread_cond_t` — condition variables

Each dongle has its own `pthread_cond_t cond`. When a coder cannot take a dongle (wrong turn, in use, or cooling), it calls:

```c
pthread_cond_wait(&d->cond, &d->mutex);
```

This **atomically releases the mutex and puts the thread to sleep**. When a dongle is released, `release_dongle()` calls:

```c
pthread_cond_broadcast(&d->cond);
```

This wakes ALL waiting coders. Each re-checks the three conditions (turn, availability, cooldown). Only the one at the front of the queue whose cooldown has passed proceeds; the rest return to sleep.

### Priority queue (min-heap)

A custom min-heap is maintained per dongle (in `queue.c`). Each entry stores a `coder_id` and a scheduling `key`:
- **FIFO**: `key = arrival_time` (ms) → smallest = earliest arrival
- **EDF**: `key = last_compile_start + time_to_burnout` → smallest = most urgent deadline

The heap ensures `O(log n)` insertion and extraction. `queue_peek_id()` lets the wait loop check in O(1) whether it is the calling coder's turn.

### Monitor thread

A separate thread runs `monitor_routine()`. It never holds a dongle mutex — it only reads coder timestamps (which are written atomically on the same core) and writes `sim->stop` under `monitor_mutex`. This avoids any priority inversion between the monitor and coder threads.

When `stop` is set, the monitor broadcasts on every dongle's condition variable to wake any thread blocked in `pthread_cond_wait`, allowing clean shutdown.

---

## Resources

### Classic references
- **The Little Book of Semaphores** — Allen B. Downey (free PDF) — covers dining philosophers and resource synchronization patterns
- **POSIX Threads Programming** — Blaise Barney, Lawrence Livermore National Laboratory: https://hpc-tutorials.llnl.gov/posix/
- **`man 7 pthreads`** — Linux man page overview of POSIX thread semantics
- **Introduction to Algorithms** (CLRS), Chapter 6 — Heapsort and priority queues
- **Operating Systems: Three Easy Pieces** — Arpaci-Dusseau (free online): https://pages.cs.wisc.edu/~remzi/OSTEP/ — Chapters on locks, condition variables, and semaphores

### AI usage
AI assistance (Claude) was used for the following tasks in this project:

- **Initial architecture brainstorming**: discussing the structure of files and which synchronization primitives to apply to which shared resources.
- **Clarifying EDF semantics**: confirming how the deadline formula (`last_compile_start + time_to_burnout`) maps to the priority key.
- **Reviewing deadlock conditions**: walking through Coffman's four conditions and confirming that resource ordering eliminates circular wait.
- **Drafting comments**: AI helped write the inline explanatory comments and visual ASCII diagrams throughout the source code.

All generated content was reviewed, understood, tested, and modified before inclusion. The core logic, data structures, and thread synchronization decisions were validated manually and through peer review.
