#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>

time_t start_time;
int shared_resource;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int active_thread_id = 0;
pthread_t active_thread;

void *process1(void *args)
{
    sleep(1);
    int attempts = 1;
    while (1)
    {
        int thread_id = *((int *)args);
        printf("Thread %d is attempting to access the resource.\n", thread_id);
        int lock_status = pthread_mutex_trylock(&mutex);

        if (lock_status == 0)
        {
            attempts = 1; // reset attempts
            printf("||---Thread %d has gained access to the resource.---||\n", thread_id);
            start_time = time(NULL);
            active_thread = pthread_self();
            active_thread_id = thread_id;
            printf("Thread %d write: writing...\n", thread_id);

            sleep(rand() % (10 - 1 + 1) + 1); // random sleep time from 1-10
            shared_resource = rand() % (10 - 1 + 1) + 1;
            printf("||---Thread %d has released the resource.---||\n", thread_id);
            pthread_mutex_unlock(&mutex);
            sleep(1);
        }
        else
        {
            printf("Thread %d is unable to access the resource. Attempt %d\n", thread_id, attempts);
            sleep(1);   // sleep for 1 second before retrying to access
            attempts++; // increment attempts
        }
    }
}

// checks timer to see if 500ms has passed since shared_resource has been accessed
void *watchdog(void *args)
{
    sleep(1);
    while (1)
    {
        time_t current_time = time(NULL);
        printf("Watchdog: Resource has been in use for %ld seconds.\n", current_time - start_time);
        if ((current_time - start_time) >= 3)
        {
            printf(" ~ Watchdog: Timeout for thread %d. Forced to release resource. ~ \n", active_thread_id);
            pthread_cancel(active_thread);
            pthread_join(active_thread, NULL);
            pthread_create(&active_thread, NULL, process1, &active_thread_id);

            pthread_detach(active_thread);
            pthread_mutex_unlock(&mutex);
            start_time = time(NULL);
        }
        sleep(1);
    }
    return NULL;
}

// mcdaondlsds
int main() // mane funktion
{
    // pthread ids
    pthread_t id1, id2, id3;

    int thread_id1 = 1;
    int thread_id2 = 2;
    int thread_id3 = 3;

    pthread_create(&id1, NULL, process1, &thread_id1); // first thread
    pthread_create(&id2, NULL, process1, &thread_id2); // second thread
    pthread_create(&id3, NULL, watchdog, &thread_id3); // watchdog thread

    pthread_join(id1, NULL);
    pthread_join(id2, NULL);
    pthread_join(id3, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
