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


