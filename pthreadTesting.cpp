#include <stdio.h>
#include <iostream>
#include <pthread.h>

using namespace std;

void *busy(void *ptr)
{
    puts("hello world");
    return NULL;
}

int main()
{
    pthread_t id;
    pthread_create(&id, NULL, busy, "Hi");

    while (true)
    {
    }
    return 0;
}