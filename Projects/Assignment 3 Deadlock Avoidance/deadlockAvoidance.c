#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>


sem_t resource_semaphore; // semefore
int shared_resource;
#define RT 4


void* process1(void* args)
{
    struct timespec ts; 
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += RT;


    if(sem_timedwait(&resource_semaphore, &ts) == 0){
        printf("Resource entered by thead %i.\n", (int)pthread_self());
        sleep(3);
        printf("Thread %d released resource.\n", (int)pthread_self());
        sem_post(&resource_semaphore);
        shared_resource = rand();
    }
    else{
        if(errno == ETIMEDOUT){
            printf("Thread %d starved (timeout reached). Restarting...\n", (int)pthread_self());
        }
        else{
            perror("sem_timedwait failed");
        }
    }
}
   
    // mcdaondlsds
int main() //mane funktion
    {

        sem_init(&resource_semaphore, 0, 1);
    
        // pthread ids
        pthread_t id1, id2;

        pthread_create(&id1, NULL, process1, NULL);
        
        pthread_create(&id2, NULL, process1, NULL);

        pthread_join(id1, NULL);

        pthread_join(id2, NULL);

        
        
    }
    
    
