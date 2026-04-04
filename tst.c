// PHASE 1: C Programming Fundamentals (Refresh)
// 1.1 Struct & typedef
// #include <stdio.h>


// typedef struct sPersong
// {
//     char *name;
//     int age;
// }t_Persong;


// int main(void)
// {
//     t_Persong p1;

//     p1.age = 10;
//     p1.name = "youness";

//     printf("age = %d , name = %s\n", p1.age, p1.name);


//     return (0);
// }


// 1.2 Pointer to Stuctur.
// #include <stdio.h>

// typedef struct sPersong
// {
//     char *name;
//     int age;
// }t_Persong;


// int main(void)
// {
//     t_Persong p1;


//     p1.age = 10;
//     p1.name = "youness";

//     t_Persong *ptr;
//     t_Persong **ptr2;
    
//     ptr = &p1;

//     ptr2 = &ptr;

//     printf("age = %d , name = %s\n", p1.age, p1.name);
//     printf("age = %d , name = %s\n", (*ptr).age, (*ptr).name);
//     printf("age = %d , name = %s\n", (*(*ptr2)).age, (*(*ptr2)).name);    ?   ?   ?   ?   ?


//     return (0);
// }


// 1.3 Daynamic memory alloction

// #include <stdio.h>
// #include <stdlib.h>

// typedef struct sPersong
// {
//     char *name;
//     int age;
// }t_Persong;


// int main(void)
// {
//     // the [] , -> and * all of them they dereference


//     // Single Object
//     t_Persong *s_ptr;
    
//     s_ptr = malloc(sizeof(t_Persong));

//     (*s_ptr).age = 10;
//     s_ptr->name = "youness";

//     printf("name = %s, age = %d\n", s_ptr->name, s_ptr->age);

//     free(s_ptr);

//     // Multiple Objects

//     t_Persong *m_ptr;
    
//     m_ptr = malloc(sizeof(t_Persong)*3);

//     (*(m_ptr + 0)).age = 1;
//     (m_ptr[0]).name = "one";
    
//     (*(m_ptr + 1)).age = 2;
//     (m_ptr[1]).name = "two";
    
//     (m_ptr + 2)->age = 3;
//     (m_ptr[2]).name = "three";
//     printf("name = %s, age = %d\n", (m_ptr[0]).name, (m_ptr[0]).age);
//     printf("name = %s, age = %d\n", (m_ptr[1]).name, (m_ptr[1]).age);
//     printf("name = %s, age = %d\n", (m_ptr[2]).name, (m_ptr[2]).age);

//     free(m_ptr);
// }






// PHASE 2: POSIX Threads (pthread) - Core Concepts

// 2.1 What is a Thread ?

// A thread is an independent execution flow within a process

// 2.2 Creating Threads

// #include <pthread.h>
// #include <stdio.h>

// void *thread_function(void *arg)
// {
//     int *value = (int *)arg;
//     printf("Thread received: %d\n", *value);
//     return NULL;
// }

// void main(void)
// {

//     pthread_t thread_id;
//     int data = 42;
    
//     /* Create thread: function, argument */
//     pthread_create(&thread_id, NULL, thread_function, &data);
    

//     /* Wait for thread to finish */
//     pthread_join(thread_id, NULL);
    
//     // return 0;
// }

// This code is meaningless as threading

// #include <pthread.h>
// #include <stdio.h>

// int main(void)
// {
//     pthread_t t1, t2;
//     t_thread_data d1 = {1, "Hello", 0};
//     t_thread_data d2 = {2, "World", 0};
    
//     pthread_create(&t1, NULL, worker, &d1);
//     pthread_join(t1, NULL);
    
//     pthread_create(&t2, NULL, worker, &d2);
//     pthread_join(t2, NULL);
    
//     printf("Results: %d, %d\n", d1.result, d2.result);
//     return 0;
// }

// 2.3 Thread Argements - Passing Data to Function.

// #include <stdio.h>
// #include <pthread.h>

// typedef struct s_thread_data {
//     int thread_num;
//     char *message;
//     int result;
// } t_thread_data;

// void *worker(void *arg)
// {
//     t_thread_data *data = (t_thread_data *)arg;
//     printf("Thread %d: %s\n", (*data).thread_num, (*data).message);
//     (*data).result = (*data).thread_num * 10;  /* Can modify shared struct */
//     return NULL;
// }

// int main(void)
// {
//     pthread_t t1, t2;
//     t_thread_data d1 = {1, "Hello", 0};
//     t_thread_data d2 = {2, "World", 0};
    
//     pthread_create(&t1, NULL, worker, &d1);
//     pthread_create(&t2, NULL, worker, &d2);
    
//     pthread_join(t1, NULL);
//     pthread_join(t2, NULL);
    
//     printf("Results: %d, %d\n", d1.result, d2.result);
//     return 0;
// }


// PHASE 3: Thread Synchronization - Mutexes


// #include <pthread.h>
// #include <stdio.h>
// #include <unistd.h>

// void *logger()
// {
//     while (1)
//     {
//         printf("still alive...\n");
//         sleep(1);
//     }
//     return NULL;
// }

// int main(void)
// {
    //     pthread_t log_thread;

    //     pthread_create(&log_thread, NULL, logger, NULL);
    
    //     // do the real work here
    //     printf("working...\n");
    //     sleep(3);
    //     printf("done.\n");
    
    //     return 0; // logger thread dies here automatically
    // }
    
    
// #include <pthread.h>
// #include <stdio.h>
    
// pthread_mutex_t lock;
// int counter = 0;
    
// void *increment()
// {
//     for (int i = 0; i < 20; i++) {

//         pthread_mutex_lock(&lock);    
//         counter++;
//         pthread_mutex_unlock(&lock);  

//     }
//     return NULL;
// }
    
// int main(void)
// {
//     pthread_t t1, t2;

//     pthread_mutex_init(&lock, NULL);
    
//     pthread_create(&t1, NULL, increment, NULL);
//     pthread_create(&t2, NULL, increment, NULL);
    
//     pthread_join(t1, NULL);
//     pthread_join(t2, NULL);
    
//     printf("Counter: %d\n", counter);  
    
//     pthread_mutex_destroy(&lock);

//     return 0;
// }

// PHASE 4: Thread Synchronization - Condition Variables


// 4.1 Why Condition Variables?

// Mutexes alone are inefficient for "wait until condition is true"
// scenarios. Busy-waiting wastes CPU


//  BAD: Busy waiting 
//     while (!condition) {
//          Spin and waste CPU 
// }



// #include <pthread.h>
// #include <stdio.h>
// #include <unistd.h>

// pthread_mutex_t lock;
// pthread_cond_t cond;

// int counter = 0;

// //  Thread that WAITS
// void *waiter(void *arg)
// {
//     pthread_mutex_lock(&lock);

//     printf("Waiter: waiting for counter to become > 0...\n");


//     pthread_cond_wait(&cond, &lock);
    

//     printf("Waiter: counter is now %d\n", counter);

//     pthread_mutex_unlock(&lock);
//     return NULL;
// }

// //  Thread that SIGNALS
// void *signaler(void *arg)
// {
//     sleep(2); // simulate delay

//     pthread_mutex_lock(&lock);

//     counter = 5;
//     printf("Signaler: counter set to %d\n", counter);

//     int result = 2;result = pthread_cond_signal(&cond);
//     printf("\n\n%d\n\n", result);
//     pthread_mutex_unlock(&lock);
//     return NULL;
// }

// int main(void)
// {
//     pthread_t t1, t2;

//     pthread_mutex_init(&lock, NULL);
//     pthread_cond_init(&cond, NULL);

//     pthread_create(&t1, NULL, waiter, NULL);
//     pthread_create(&t2, NULL, signaler, NULL);

//     pthread_join(t1, NULL);
//     pthread_join(t2, NULL);

//     pthread_mutex_destroy(&lock);
//     pthread_cond_destroy(&cond);

//     return 0;
// }



// #include <pthread.h>

// pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
// pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
// int ready = 0;

// void *waiter(void *arg)
// {
//     (void)arg;
//     pthread_mutex_lock(&lock);
    
//     while (!ready) {  /* MUST check in loop (spurious wakeups) */
//         pthread_cond_wait(&cond, &lock);  /* Atomically unlock and wait */
//     }
//     /* When signaled, lock is reacquired */
//     printf("Proceeding!\n");
    
//     pthread_mutex_unlock(&lock);
//     return NULL;
// }

// void *signaler(void *arg)
// {
//     (void)arg;
//     sleep(1);

//     pthread_mutex_lock(&lock);
//     ready = 1;
//     pthread_mutex_unlock(&lock);
    
//     pthread_cond_broadcast(&cond);  /* Wake all waiters */
//     pthread_cond_signal(&cond);

//     /* Or pthread_cond_signal(&cond) to wake one */
    
//     return NULL;
// }

// 4.4 Timed Wait (For Burnout Detection)

// Exercise 1: Print the current time every second in a loop 5 times.

// #include <stdio.h>
// #include <unistd.h>
// #include <time.h>

// int main(void)
// {
//     time_t t1 = time(NULL);
//     sleep(1);
//     time_t t2 = time(NULL);
//     sleep(1);
//     time_t t3 = time(NULL);
//     sleep(1);
//     time_t t4 = time(NULL);
//     sleep(1);
//     time_t t5 = time(NULL);
    
//     printf("%ld\n", t1);
//     printf("%ld\n", t2);
//     printf("%ld\n", t3);
//     printf("%ld\n", t4);
//     printf("%ld\n", t5);

//     return 0;
// }

// Exercise 2: Write a program that measures how long it takes to run a for-loop 1,000,000 times using gettimeofday.

// #include <stdio.h>
// #include <sys/time.h>

// int main() {

//     struct timeval tv;
//     struct timeval tv1;

//     gettimeofday(&tv, NULL);

//     for(int i = 0; i < 1000000000; i++);

//     gettimeofday(&tv1, NULL);
//     printf("how long it takes to run a for-loop 1,000,000: %ld\n", (((tv1.tv_sec - tv.tv_sec) * 1000000) + (tv1.tv_usec - tv.tv_usec)));

//     return 0;
// }

// #include <stdio.h>
// #include <pthread.h>
// #include <sys/time.h>
// #include <errno.h>
// #include <unistd.h>
// int ready = 0;
// pthread_mutex_t lock;
// pthread_cond_t cond;
// void *func()
// {
//     sleep(2);
//     pthread_cond_signal(&cond);
//     return NULL;
// }

// void *func2()
// {
//     struct timeval tv;
//     struct timespec ts;

//     pthread_t th;
//     pthread_mutex_init(&lock, NULL);
//     pthread_cond_init(&cond, NULL);

//     gettimeofday(&tv, NULL); // Get current time
//     ts.tv_sec = tv.tv_sec + 10; // 3 seconds timeout
//     ts.tv_nsec = tv.tv_usec * 1000;

//     pthread_mutex_lock(&lock);
//     int result = 0;
//     while (!ready && result != ETIMEDOUT) {
//         result = pthread_cond_timedwait(&cond, &lock, &ts);
//     }
//     if (result == ETIMEDOUT) {
//         printf("Timeout occurred!\n");
//     } else {
//         printf("Ready detected!\n");
//     }
//     pthread_mutex_destroy(&lock);
//     pthread_mutex_unlock(&lock);
//     pthread_cond_destroy(&cond);

//     return NULL;
// }
// int main() {

//     pthread_t th[2];


//     // pthread_create(&th[0], NULL, func, NULL);
//     pthread_create(&th[1], NULL, func2, NULL);
    
//     // pthread_join(th[0], NULL);
//     pthread_join(th[1], NULL);

//     return 0;
// }
#include <stdio.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    usleep(5 * 1000);// 5ms
    printf("3 seconds passed\n"); 
    return 0;
}
