#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>


const int counter = 1000000;
int var = 0;
bool is_finiche = false;

void *inc()
{
    for (int i = 0; i < counter; i++)
    {
        var++;
    }

    is_finiche = true;
    return NULL;
}

void *dec()
{
    while (!is_finiche);

    for (int i = 0; i < counter; i++)
    {
        var--;
    }

    return NULL;
}

int main(void)
{

    pthread_t th[2];

    pthread_create(&th[0], NULL, inc, NULL);
    pthread_create(&th[1], NULL, dec, NULL);
    
    pthread_join(th[0], NULL);
    pthread_join(th[1], NULL);

    printf("var = %d\n",var);

    return 0;
}