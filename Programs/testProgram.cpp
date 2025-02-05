#include <stdio.h>
#include <pthread.h>
void *busy(void *ptr)
{
    puts("Hello World");
    return NULL;
}

void *busy1(void *ptr)
{
    puts("I am hanging you!!!");
    return NULL;
}

int main()
{
    pthread_t id;
    pthread_create(&id, NULL, busy, (void *)"Hi");
    pthread_create(&id, NULL, busy1, (void *)"Ola");
    
    while (1)
    {
    }

    return 0;
}