#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
pthread_cond_t cond;
pthread_mutex_t mutex;

int you;
void *func1()
{

    pthread_mutex_lock(&mutex);

    
    pthread_cond_wait(&cond, &mutex);
    
    pthread_mutex_unlock(&mutex);
    
    
    return NULL;
}

void *func2()
{

    pthread_mutex_lock(&mutex);

    
    pthread_cond_wait(&cond, &mutex);
    
    pthread_mutex_unlock(&mutex);
    
    
    return NULL;
}

int main(void)
{
    pthread_t th[2];
    
    
    
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    
    
    pthread_create(&th[0], NULL, func1, NULL);
    pthread_create(&th[0], NULL, func2, NULL);

    pthread_create(&th[1], NULL, func2, NULL);
    pthread_create(&th[1], NULL, func1, NULL);

    sleep(2);
    
    you++;

    while (you != 1)
    {
        printf("a = %d\n", you);
    }
    
    pthread_cond_broadcast(&cond);
    
    pthread_join(th[0], NULL);
    pthread_join(th[1], NULL);
    
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    
    return 0;
    
}
