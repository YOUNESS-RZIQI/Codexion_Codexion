# Codexion Learning Guide - From A to Z

Complete learning path to solve the Codexion project.

---

## PHASE 1: C Programming Fundamentals (Refresh)

### 1.1 Structures and Typedef
```c
/* Basic struct with typedef */
typedef struct s_person {
    char *name;
    int age;
} t_person;

/* Usage */
t_person p;
p.name = "Alice";
p.age = 25;
```

### 1.2 Pointers to Structures
```c
t_person *ptr = &p;
(*ptr).age = 26;     /* Dereference then access */
ptr->age = 26;       /* Arrow operator (same thing) */

/* Pointer to pointer (for arrays) */
t_person **people;   /* Array of person pointers */
```

### 1.3 Dynamic Memory Allocation
```c
/* Single object */
t_person *p = malloc(sizeof(t_person));
if (p == NULL) return NULL;  /* ALWAYS check malloc */

/* Array of objects */
t_person *arr = malloc(sizeof(t_person) * 10);

/* Free memory */
free(p);
free(arr);
```

### 1.4 C89 Specific Rules (Important for Norm)
```c
void example(void)
{
    int i;           /* Declare ALL variables at the TOP */
    int j;
    
    i = 0;
    for (j = 0; j < 10; j++) {  /* C89 allows declaration in for init */
        /* code */
    }
    
    /* NO // comments in C89 - use /* */ only */
}
```

---

## PHASE 2: POSIX Threads (pthread) - Core Concepts

### 2.1 What is a Thread?
- A thread is an independent execution flow within a process
- Multiple threads share the same memory space
- Each thread has its own stack but shares heap and global variables
- **CRITICAL**: Codexion forbids global variables - pass context via arguments

### 2.2 Creating Threads
```c
#include <pthread.h>

void *thread_function(void *arg)
{
    int *value = (int *)arg;
    printf("Thread received: %d\n", *value);
    return NULL;
}

int main(void)
{
    pthread_t thread_id;
    int data = 42;
    
    /* Create thread: function, argument */
    pthread_create(&thread_id, NULL, thread_function, &data);
    
    /* Wait for thread to finish */
    pthread_join(thread_id, NULL);
    
    return 0;
}
```

### 2.3 Thread Arguments - Passing Data
```c
typedef struct s_thread_data {
    int thread_num;
    char *message;
    int result;
} t_thread_data;

void *worker(void *arg)
{
    t_thread_data *data = (t_thread_data *)arg;
    printf("Thread %d: %s\n", data->thread_num, data->message);
    data->result = data->thread_num * 10;  /* Can modify shared struct */
    return NULL;
}

int main(void)
{
    pthread_t t1, t2;
    t_thread_data d1 = {1, "Hello", 0};
    t_thread_data d2 = {2, "World", 0};
    
    pthread_create(&t1, NULL, worker, &d1);
    pthread_create(&t2, NULL, worker, &d2);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    printf("Results: %d, %d\n", d1.result, d2.result);
    return 0;
}
```

---

## PHASE 3: Thread Synchronization - Mutexes

### 3.1 Race Condition Problem
```c
int counter = 0;

void *increment(void *arg)
{
    (void)arg;
    for (int i = 0; i < 100000; i++) {
        counter++;  /* DANGER: Not atomic! */
    }
    return NULL;
}
/* Two threads running this will NOT reach 200000! */
```

### 3.2 Mutex Basics
```c
#include <pthread.h>

pthread_mutex_t lock;  /* Declare mutex */
int counter = 0;

void *increment(void *arg)
{
    (void)arg;
    for (int i = 0; i < 100000; i++) {
        pthread_mutex_lock(&lock);    /* Acquire lock */
        counter++;                     /* Critical section */
        pthread_mutex_unlock(&lock);  /* Release lock */
    }
    return NULL;
}

int main(void)
{
    pthread_t t1, t2;
    
    /* Initialize mutex */
    pthread_mutex_init(&lock, NULL);
    
    pthread_create(&t1, NULL, increment, NULL);
    pthread_create(&t2, NULL, increment, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    printf("Counter: %d\n", counter);  /* Now correctly 200000 */
    
    /* Destroy mutex */
    pthread_mutex_destroy(&lock);
    return 0;
}
```

### 3.3 Protecting Shared Resources
```c
typedef struct s_shared {
    int value;
    pthread_mutex_t mutex;  /* Mutex embedded in shared data */
} t_shared;

void modify_shared(t_shared *shared, int new_val)
{
    pthread_mutex_lock(&shared->mutex);
    shared->value = new_val;  /* Protected access */
    pthread_mutex_unlock(&shared->mutex);
}
```

---

## PHASE 4: Thread Synchronization - Condition Variables

### 4.1 Why Condition Variables?
Mutexes alone are inefficient for "wait until condition is true" scenarios. Busy-waiting wastes CPU:
```c
/* BAD: Busy waiting */
while (!condition) {
    /* Spin and waste CPU */
}
```

### 4.2 Condition Variable Basics
```c
#include <pthread.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int ready = 0;

void *waiter(void *arg)
{
    (void)arg;
    pthread_mutex_lock(&lock);
    
    while (!ready) {  /* MUST check in loop (spurious wakeups) */
        pthread_cond_wait(&cond, &lock);  /* Atomically unlock and wait */
    }
    /* When signaled, lock is reacquired */
    printf("Proceeding!\n");
    
    pthread_mutex_unlock(&lock);
    return NULL;
}

void *signaler(void *arg)
{
    (void)arg;
    sleep(1);
    
    pthread_mutex_lock(&lock);
    ready = 1;
    pthread_mutex_unlock(&lock);
    
    pthread_cond_broadcast(&cond);  /* Wake all waiters */
    /* Or pthread_cond_signal(&cond) to wake one */
    
    return NULL;
}
```

### 4.3 The Pattern for Codexion (Dongle Request)
```c
typedef struct s_dongle {
    pthread_mutex_t lock;
    pthread_cond_t available;  /* Wait here when dongle unavailable */
    int is_available;
} t_dongle;

void request_dongle(t_dongle *dongle)
{
    pthread_mutex_lock(&dongle->lock);
    
    while (!dongle->is_available) {
        /* Release lock, wait for signal, reacquire lock */
        pthread_cond_wait(&dongle->available, &dongle->lock);
    }
    
    dongle->is_available = 0;  /* Take the dongle */
    pthread_mutex_unlock(&dongle->lock);
}

void release_dongle(t_dongle *dongle)
{
    pthread_mutex_lock(&dongle->lock);
    
    dongle->is_available = 1;
    pthread_cond_signal(&dongle->available);  /* Wake one waiter */
    
    pthread_mutex_unlock(&dongle->lock);
}
```

### 4.4 Timed Wait (For Burnout Detection)
```c
#include <sys/time.h>

int wait_with_timeout(t_dongle *dongle, long timeout_ms)
{
    struct timespec ts;
    struct timeval tv;
    int result;
    
    gettimeofday(&tv, NULL);
    ts.tv_sec = tv.tv_sec + (timeout_ms / 1000);
    ts.tv_nsec = (tv.tv_usec + (timeout_ms % 1000) * 1000) * 1000;
    
    pthread_mutex_lock(&dongle->lock);
    
    while (!dongle->is_available) {
        result = pthread_cond_timedwait(&dongle->available, 
                                        &dongle->lock, &ts);
        if (result == ETIMEDOUT) {
            pthread_mutex_unlock(&dongle->lock);
            return -1;  /* Timeout! */
        }
    }
    
    dongle->is_available = 0;
    pthread_mutex_unlock(&dongle->lock);
    return 0;
}
```

---

## PHASE 5: Timing in C - gettimeofday and usleep

### 5.1 gettimeofday() - High Precision Timing
```c
#include <sys/time.h>

long get_timestamp_ms(void)
{
    struct timeval tv;
    
    gettimeofday(&tv, NULL);
    
    /* tv.tv_sec = seconds since epoch */
    /* tv.tv_usec = microseconds (0-999999) */
    
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

/* Usage */
long start = get_timestamp_ms();
/* ... do work ... */
long end = get_timestamp_ms();
long elapsed = end - start;
printf("Elapsed: %ld ms\n", elapsed);
```

### 5.2 usleep() - Microsecond Sleep
```c
#include <unistd.h>

/* Sleep for 100 milliseconds */
usleep(100 * 1000);  /* 100,000 microseconds = 100ms */

/* Sleep for 1 second */
usleep(1000000);     /* 1,000,000 microseconds = 1s */

/* NOTE: usleep may sleep longer than requested due to scheduling */
```

### 5.3 Implementing Time-Limited Operations
```c
void precise_sleep_until(long target_time_ms, t_shared *shared)
{
    long current;
    long remaining;
    
    while (shared->running) {  /* Check if simulation stopped */
        current = get_timestamp_ms();
        remaining = target_time_ms - current;
        
        if (remaining <= 0)
            break;
        
        if (remaining > 10)
            usleep(1000);  /* Sleep 1ms if still far from target */
        else
            usleep(100);   /* Short sleep when close */
    }
}
```

---

## PHASE 6: Data Structures - Min Heap Implementation

### 6.1 What is a Min Heap?
- Complete binary tree where parent ≤ children
- Root is always the minimum element
- Perfect for priority queues (EDF scheduling)
- Operations: insert (O(log n)), extract_min (O(log n))

### 6.2 Heap Array Representation
```
Index:    0    1    2    3    4    5    6
Value:    1    3    5    7    9    8    6

Tree view:
          1
        /   \
       3     5
      / \   / \
     7   9 8   6

Parent of i: (i - 1) / 2
Left child:  2 * i + 1
Right child: 2 * i + 2
```

### 6.3 Heap Data Structure
```c
typedef struct s_heap_node {
    int coder_id;
    long deadline;  /* Priority key - earlier = higher priority */
} t_heap_node;

typedef struct s_heap {
    t_heap_node *nodes;  /* Dynamic array */
    int size;            /* Current number of elements */
    int capacity;        /* Allocated capacity */
} t_heap;
```

### 6.4 Heap Operations
```c
#include <stdlib.h>

/* Create heap */
t_heap *heap_create(int capacity)
{
    t_heap *h = malloc(sizeof(t_heap));
    if (!h) return NULL;
    
    h->nodes = malloc(sizeof(t_heap_node) * capacity);
    if (!h->nodes) {
        free(h);
        return NULL;
    }
    
    h->size = 0;
    h->capacity = capacity;
    return h;
}

/* Swap two nodes */
void heap_swap(t_heap_node *a, t_heap_node *b)
{
    t_heap_node tmp = *a;
    *a = *b;
    *b = tmp;
}

/* Sift up - after insertion */
void heap_sift_up(t_heap *h, int index)
{
    int parent;
    
    while (index > 0) {
        parent = (index - 1) / 2;
        
        /* If current has earlier deadline than parent, swap */
        if (h->nodes[index].deadline < h->nodes[parent].deadline) {
            heap_swap(&h->nodes[index], &h->nodes[parent]);
            index = parent;
        } else {
            break;
        }
    }
}

/* Insert into heap */
int heap_insert(t_heap *h, int coder_id, long deadline)
{
    if (h->size >= h->capacity)
        return -1;  /* Full */
    
    /* Add at end */
    h->nodes[h->size].coder_id = coder_id;
    h->nodes[h->size].deadline = deadline;
    h->size++;
    
    /* Restore heap property */
    heap_sift_up(h, h->size - 1);
    return 0;
}

/* Sift down - after extraction */
void heap_sift_down(t_heap *h, int index)
{
    int left, right, smallest;
    
    while (1) {
        left = 2 * index + 1;
        right = 2 * index + 2;
        smallest = index;
        
        if (left < h->size && 
            h->nodes[left].deadline < h->nodes[smallest].deadline)
            smallest = left;
        
        if (right < h->size && 
            h->nodes[right].deadline < h->nodes[smallest].deadline)
            smallest = right;
        
        if (smallest == index)
            break;
        
        heap_swap(&h->nodes[index], &h->nodes[smallest]);
        index = smallest;
    }
}

/* Extract minimum */
int heap_extract_min(t_heap *h, int *coder_id, long *deadline)
{
    if (h->size <= 0)
        return -1;  /* Empty */
    
    /* Get root */
    *coder_id = h->nodes[0].coder_id;
    *deadline = h->nodes[0].deadline;
    
    /* Move last to root */
    h->size--;
    if (h->size > 0) {
        h->nodes[0] = h->nodes[h->size];
        heap_sift_down(h, 0);
    }
    
    return 0;
}

/* Check if empty */
int heap_is_empty(t_heap *h)
{
    return h->size == 0;
}

/* Destroy heap */
void heap_destroy(t_heap *h)
{
    if (h) {
        free(h->nodes);
        free(h);
    }
}
```

---

## PHASE 7: Dining Philosophers Problem Theory

### 7.1 The Problem
- N philosophers sit around a table
- Each needs 2 forks to eat
- Each philosopher alternates: think → eat → think
- **Deadlock risk**: All pick up left fork, wait forever for right

### 7.2 Four Coffman Conditions for Deadlock
1. **Mutual Exclusion**: Resources are non-sharable (forks/dongles)
2. **Hold and Wait**: Thread holds one resource while waiting for another
3. **No Preemption**: Resources cannot be forcibly taken
4. **Circular Wait**: Circular chain of threads waiting

**To prevent deadlock, break at least one condition.**

### 7.3 Solutions

**Solution 1: Resource Hierarchy (Use this for Codexion)**
```c
void acquire_forks(t_philosopher *p)
{
    t_fork *first, *second;
    
    /* Always acquire lower-numbered fork first */
    if (p->left_fork->id < p->right_fork->id) {
        first = p->left_fork;
        second = p->right_fork;
    } else {
        first = p->right_fork;
        second = p->left_fork;
    }
    
    pthread_mutex_lock(&first->mutex);
    pthread_mutex_lock(&second->mutex);
}
```

**Solution 2: Try-Lock with Backoff**
```c
void acquire_forks_trylock(t_philosopher *p)
{
    while (1) {
        pthread_mutex_lock(&p->left_fork->mutex);
        
        if (pthread_mutex_trylock(&p->right_fork->mutex) == 0) {
            return;  /* Got both! */
        }
        
        /* Failed to get right, release left and retry */
        pthread_mutex_unlock(&p->left_fork->mutex);
        usleep(1000);  /* Brief pause before retry */
    }
}
```

---

## PHASE 8: Deadlock Prevention Strategies

### 8.1 For Codexion: Resource Ordering
```c
/* Coders sit in circle: each has left and right dongle */
/* Left dongle ID = coder ID */
/* Right dongle ID = (coder ID % N) + 1 */

void acquire_dongles(t_coder *coder)
{
    t_dongle *first;
    t_dongle *second;
    int left_id;
    int right_id;
    
    left_id = coder->left->id;
    right_id = coder->right->id;
    
    /* Always pick lower ID first - breaks circular wait */
    if (left_id < right_id) {
        first = coder->left;
        second = coder->right;
    } else {
        first = coder->right;
        second = coder->left;
    }
    
    /* Wait for scheduling (FIFO/EDF) for first dongle */
    wait_for_dongle(first, coder);
    
    /* Wait for scheduling for second dongle */
    wait_for_dongle(second, coder);
}
```

### 8.2 One Coder Edge Case
```c
/* When N=1, coder needs the SAME dongle twice! */
/* Special handling required */
void acquire_dongles_single(t_coder *coder)
{
    if (coder->shared->num_coders == 1) {
        /* Just need one dongle - take it */
        wait_for_dongle(coder->left, coder);
    } else {
        /* Normal case */
        acquire_dongles(coder);
    }
}
```

---

## PHASE 9: Project Architecture & Data Structures

### 9.1 Complete Data Structure Design
```c
#ifndef CODEXION_H
#define CODEXION_H

#include <pthread.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* Forward declarations */
struct s_dongle;
struct s_coder;
struct s_shared;

/* ==================== PRIORITY QUEUE (FOR EDF) ==================== */

typedef struct s_heap_node {
    int coder_id;
    long deadline;
} t_heap_node;

typedef struct s_heap {
    t_heap_node *nodes;
    int size;
    int capacity;
} t_heap;

/* ==================== FIFO QUEUE NODE ==================== */

typedef struct s_queue_node {
    int coder_id;
    long request_time;
    pthread_cond_t cond;
    struct s_queue_node *next;
} t_queue_node;

typedef struct s_queue {
    t_queue_node *head;
    t_queue_node *tail;
    int size;
} t_queue;

/* ==================== DONGLE ==================== */

typedef struct s_dongle {
    int id;
    pthread_mutex_t lock;
    pthread_cond_t cond;
    
    int is_available;
    long cooldown_until;  /* Timestamp when available again */
    
    /* Scheduling queue - one of these based on mode */
    t_queue *fifo_queue;  /* For FIFO scheduling */
    t_heap *edf_heap;     /* For EDF scheduling */
} t_dongle;

/* ==================== CODER ==================== */

typedef struct s_coder {
    int id;
    pthread_t thread;
    
    /* Timing */
    long last_compile_start;
    long simulation_start;
    
    /* Progress */
    int compile_count;
    int has_burned_out;
    
    /* Resources */
    t_dongle *left;
    t_dongle *right;
    
    /* Reference to shared state */
    struct s_shared *shared;
} t_coder;

/* ==================== SHARED STATE ==================== */

typedef struct s_shared {
    /* Parameters */
    int num_coders;
    long time_to_burnout;
    long time_to_compile;
    long time_to_debug;
    long time_to_refactor;
    int compiles_required;
    long dongle_cooldown;
    int use_edf;  /* 0 = FIFO, 1 = EDF */
    
    /* Resources */
    t_dongle **dongles;
    t_coder **coders;
    
    /* Synchronization */
    pthread_mutex_t log_mutex;
    pthread_mutex_t stop_mutex;
    
    /* State */
    int running;
    long start_time;
    int burned_out_coder;
} t_shared;

/* ==================== FUNCTION PROTOTYPES ==================== */

/* Heap operations */
t_heap *heap_create(int capacity);
void heap_destroy(t_heap *h);
int heap_insert(t_heap *h, int coder_id, long deadline);
int heap_extract_min(t_heap *h, int *coder_id, long *deadline);
int heap_is_empty(t_heap *h);

/* Queue operations */
t_queue *queue_create(void);
void queue_destroy(t_queue *q);
void queue_enqueue(t_queue *q, int coder_id, long request_time, pthread_cond_t *cond);
t_queue_node *queue_dequeue(t_queue *q);
int queue_is_empty(t_queue *q);

/* Dongle operations */
void wait_for_dongle(t_dongle *dongle, t_coder *coder);
void release_dongle(t_dongle *dongle, t_shared *shared);

/* Coder thread */
void *coder_routine(void *arg);
void acquire_dongles(t_coder *coder);
void release_dongles(t_coder *coder);

/* Monitor thread */
void *monitor_routine(void *arg);

/* Logging */
void log_state(t_shared *shared, int coder_id, const char *state);

/* Timing */
long get_timestamp_ms(void);
long get_elapsed_ms(t_shared *shared);
void precise_sleep(long ms, t_shared *shared);

/* Initialization */
t_shared *init_shared(int argc, char **argv);
void cleanup_shared(t_shared *shared);

#endif
```

---

## PHASE 10: Implementation - Core Components

### 10.1 Coder Thread Routine
```c
void *coder_routine(void *arg)
{
    t_coder *coder;
    t_shared *shared;
    
    coder = (t_coder *)arg;
    shared = coder->shared;
    
    /* Initialize timing */
    coder->simulation_start = shared->start_time;
    coder->last_compile_start = 0;  /* Will be set on first compile */
    
    while (shared->running) {
        /* === PHASE 1: ACQUIRE DONGLES === */
        acquire_dongles(coder);
        
        /* === PHASE 2: COMPILING === */
        coder->last_compile_start = get_elapsed_ms(shared);
        coder->compile_count++;
        
        log_state(shared, coder->id, "has taken a dongle");
        log_state(shared, coder->id, "has taken a dongle");
        log_state(shared, coder->id, "is compiling");
        
        precise_sleep(shared->time_to_compile, shared);
        
        /* === PHASE 3: RELEASE DONGLES === */
        release_dongles(coder);
        
        /* Check if all coders completed required compiles */
        pthread_mutex_lock(&shared->stop_mutex);
        if (all_coders_done(shared)) {
            shared->running = 0;
            pthread_mutex_unlock(&shared->stop_mutex);
            break;
        }
        pthread_mutex_unlock(&shared->stop_mutex);
        
        /* === PHASE 4: DEBUGGING === */
        log_state(shared, coder->id, "is debugging");
        precise_sleep(shared->time_to_debug, shared);
        
        /* === PHASE 5: REFACTORING === */
        log_state(shared, coder->id, "is refactoring");
        precise_sleep(shared->time_to_refactor, shared);
        
        /* Loop back to acquire dongles */
    }
    
    return NULL;
}
```

### 10.2 Dongle Acquisition with Scheduling
```c
void wait_for_dongle(t_dongle *dongle, t_coder *coder)
{
    t_shared *shared;
    long current_time;
    long deadline;
    
    shared = coder->shared;
    
    pthread_mutex_lock(&dongle->lock);
    
    /* If not available, add to queue and wait */
    while (!dongle->is_available || 
           get_timestamp_ms() < dongle->cooldown_until) {
        
        current_time = get_timestamp_ms();
        
        /* Calculate deadline for EDF */
        if (coder->last_compile_start == 0) {
            /* First compile - use simulation start */
            deadline = shared->time_to_burnout;
        } else {
            deadline = coder->last_compile_start + shared->time_to_burnout;
        }
        
        /* Add to appropriate queue */
        if (shared->use_edf) {
            heap_insert(dongle->edf_heap, coder->id, deadline);
        } else {
            pthread_cond_t my_cond = PTHREAD_COND_INITIALIZER;
            queue_enqueue(dongle->fifo_queue, coder->id, current_time, &my_cond);
            pthread_cond_wait(&my_cond, &dongle->lock);
            pthread_cond_destroy(&my_cond);
            continue;  /* Re-check availability */
        }
        
        pthread_cond_wait(&dongle->cond, &dongle->lock);
        
        /* Remove from queue when signaled */
        if (shared->use_edf) {
            /* Heap doesn't support easy removal - check at top */
        }
    }
    
    /* Take the dongle */
    dongle->is_available = 0;
    pthread_mutex_unlock(&dongle->lock);
}
```

### 10.3 Dongle Release with Cooldown
```c
void release_dongle(t_dongle *dongle, t_shared *shared)
{
    int next_coder;
    long next_deadline;
    t_queue_node *node;
    
    pthread_mutex_lock(&dongle->lock);
    
    /* Set cooldown period */
    dongle->cooldown_until = get_timestamp_ms() + shared->dongle_cooldown;
    dongle->is_available = 1;
    
    /* Signal next coder in queue */
    if (shared->use_edf && !heap_is_empty(dongle->edf_heap)) {
        heap_extract_min(dongle->edf_heap, &next_coder, &next_deadline);
        pthread_cond_broadcast(&dongle->cond);  /* Wake all, they'll check */
    } else if (!shared->use_edf && !queue_is_empty(dongle->fifo_queue)) {
        node = queue_dequeue(dongle->fifo_queue);
        pthread_cond_signal(node->cond);  /* Wake specific waiter */
        free(node);
    } else {
        pthread_cond_signal(&dongle->cond);
    }
    
    pthread_mutex_unlock(&dongle->lock);
}
```

---

## PHASE 11: Implementation - Scheduling (FIFO/EDF)

### 11.1 FIFO Queue Implementation
```c
t_queue *queue_create(void)
{
    t_queue *q;
    
    q = malloc(sizeof(t_queue));
    if (!q) return NULL;
    
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

void queue_enqueue(t_queue *q, int coder_id, long request_time, 
                   pthread_cond_t *cond)
{
    t_queue_node *node;
    
    node = malloc(sizeof(t_queue_node));
    if (!node) return;
    
    node->coder_id = coder_id;
    node->request_time = request_time;
    node->cond = cond;
    node->next = NULL;
    
    if (q->tail) {
        q->tail->next = node;
    } else {
        q->head = node;
    }
    q->tail = node;
    q->size++;
}

t_queue_node *queue_dequeue(t_queue *q)
{
    t_queue_node *node;
    
    if (!q->head) return NULL;
    
    node = q->head;
    q->head = node->next;
    if (!q->head) {
        q->tail = NULL;
    }
    q->size--;
    
    return node;
}

int queue_is_empty(t_queue *q)
{
    return q->size == 0;
}

void queue_destroy(t_queue *q)
{
    t_queue_node *current;
    t_queue_node *next;
    
    if (!q) return;
    
    current = q->head;
    while (current) {
        next = current->next;
        free(current);
        current = next;
    }
    free(q);
}
```

---

## PHASE 12: Implementation - Monitor Thread

### 12.1 Burnout Detection
```c
void *monitor_routine(void *arg)
{
    t_shared *shared;
    t_coder *coder;
    long current;
    long deadline;
    int i;
    int all_done;
    
    shared = (t_shared *)arg;
    
    while (1) {
        pthread_mutex_lock(&shared->stop_mutex);
        
        if (!shared->running) {
            pthread_mutex_unlock(&shared->stop_mutex);
            break;
        }
        
        current = get_elapsed_ms(shared);
        all_done = 1;
        
        for (i = 0; i < shared->num_coders; i++) {
            coder = shared->coders[i];
            
            /* Check if compile quota met */
            if (coder->compile_count < shared->compiles_required) {
                all_done = 0;
            }
            
            /* Check for burnout */
            if (coder->last_compile_start == 0) {
                /* First compile - use simulation start */
                deadline = shared->time_to_burnout;
            } else {
                deadline = coder->last_compile_start + shared->time_to_burnout;
            }
            
            if (current > deadline) {
                /* BURNOUT DETECTED */
                shared->burned_out_coder = coder->id;
                shared->running = 0;
                
                log_state(shared, coder->id, "burned out");
                
                /* Wake all waiting threads */
                broadcast_all_dongles(shared);
                
                pthread_mutex_unlock(&shared->stop_mutex);
                return NULL;
            }
        }
        
        /* Check if all done */
        if (all_done) {
            shared->running = 0;
            pthread_mutex_unlock(&shared->stop_mutex);
            break;
        }
        
        pthread_mutex_unlock(&shared->stop_mutex);
        
        /* Short sleep for polling precision */
        usleep(100);  /* 0.1ms polling interval */
    }
    
    return NULL;
}
```

---

## PHASE 13: Testing & Debugging

### 13.1 Test Cases
```bash
# Test 1: Single coder
./codexion 1 800 200 200 200 5 0 fifo

# Test 2: Two coders (basic)
./codexion 2 800 200 200 200 5 0 fifo

# Test 3: Many coders
./codexion 5 800 200 200 200 5 0 fifo

# Test 4: With cooldown
./codexion 3 800 200 200 200 5 100 fifo

# Test 5: EDF scheduler
./codexion 3 800 200 200 200 5 0 edf

# Test 6: Tight deadline (should burnout)
./codexion 5 400 200 200 200 5 0 fifo

# Test 7: Edge case - equal times
./codexion 2 600 200 200 200 3 0 fifo
```

### 13.2 Debugging Techniques
```c
/* Add debug logging */
#ifdef DEBUG
#define debug_print(fmt, ...) \
    fprintf(stderr, "[DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
#define debug_print(fmt, ...)
#endif

/* Use in code */
debug_print("Coder %d waiting for dongle %d", coder->id, dongle->id);
```

### 13.3 Common Bugs
| Bug | Symptom | Solution |
|-----|---------|----------|
| Deadlock | Program hangs | Check resource ordering |
| Race condition | Wrong output | Verify mutex coverage |
| Memory leak | valgrind errors | Free all allocations |
| Missed burnout | Late/early log | Check timing precision |
| Interleaved logs | Mixed messages | Use log mutex |

---

## PHASE 14: Norm Compliance & Cleanup

### 14.1 Norm Checklist
- [ ] No global variables
- [ ] Function length ≤ 25 lines
- [ ] Max 4 parameters per function
- [ ] Max 5 functions per file
- [ ] `/* C89 comments only */`
- [ ] Variables declared at function start
- [ ] No assignments in conditions
- [ ] No multiple declarations: `int a, b;` is OK, `int a = 1, b = 2;` is NOT

### 14.2 File Organization
```
coders/
├── codexion.h          # All type definitions
├── main.c              # main(), init, cleanup
├── init.c              # Initialization functions
├── cleanup.c           # Memory cleanup
├── coder.c             # Coder thread routine
├── monitor.c           # Monitor thread routine
├── dongle.c            # Dongle operations
├── queue.c             # FIFO queue
├── heap.c              # Min heap
├── timing.c            # gettimeofday utilities
└── log.c               # Logging utilities
```

### 14.3 Makefile Template
```makefile
NAME = codexion

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread

SRCS = main.c init.c cleanup.c coder.c monitor.c \
       dongle.c queue.c heap.c timing.c log.c

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
```

---

## Quick Reference: Function Signatures You Need

### pthread Functions
```c
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine)(void *), void *arg);
int pthread_join(pthread_t thread, void **retval);
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr);
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex,
                           const struct timespec *abstime);
int pthread_cond_broadcast(pthread_cond_t *cond);
int pthread_cond_destroy(pthread_cond_t *cond);
```

### Timing Functions
```c
int gettimeofday(struct timeval *tv, struct timezone *tz);
int usleep(useconds_t usec);
```

---

## Summary: Learning Checklist

| Phase | Topic | Key Skills |
|-------|-------|------------|
| 1 | C Refresh | Structs, pointers, malloc/free |
| 2 | Threads | pthread_create, pthread_join |
| 3 | Mutexes | lock/unlock, protecting shared data |
| 4 | Condition Variables | wait/signal, avoiding busy-wait |
| 5 | Timing | gettimeofday, usleep |
| 6 | Heap | Min-heap implementation from scratch |
| 7 | Dining Philosophers | Understand the problem |
| 8 | Deadlock Prevention | Resource ordering |
| 9 | Architecture | Design your data structures |
| 10 | Core Implementation | Coder thread, dongle ops |
| 11 | Scheduling | FIFO queue, EDF heap |
| 12 | Monitor | Burnout detection |
| 13 | Testing | Test cases, debugging |
| 14 | Polish | Norm, cleanup, Makefile |

Start with Phase 1 and work through sequentially. Each phase builds on the previous!