#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


void *func()
{
    printf("Hello Wordl\n");
    return NULL;
}


int main(void)
{

    pthread_t th;


    pthread_create(&th, NULL, func, NULL);
    pthread_join(th, NULL);

    return 0;
}