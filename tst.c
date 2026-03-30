#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
// #include <semaphore.h>
#include <stdlib.h>

// sem_t sem;
int sem = 1;

void *func(void *ind)
{
    // sem_wait(&sem);
    while(sem == 0)
        printf("wating: %d\n", *((int*)ind));
    sem--;

    printf("Done: %d\n", *((int*)ind));

    sem++;
    // sem_post(&sem);
    return NULL;
}


int main(void)
{
    // sem_init(&sem, 0, 1);

    int num = 10;
    
    pthread_t th[num];

    for (int i = 0; i < num; i++)
    {
        int *a = malloc(sizeof(int));
        *a = i;
        pthread_create(&th[i], NULL, func, a);
    }

    for (int i = 0; i < num; i++)
    {
        pthread_join(th[i], NULL);
    }

    // sem_destroy(&sem);

    return (0);
}
