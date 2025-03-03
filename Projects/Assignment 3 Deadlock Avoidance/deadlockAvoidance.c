#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

// Global variables
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
time_t start_time;
int shared_resource;

// These track which thread currently holds the resource.
pthread_t active_thread;
int active_thread_id = 0;

// Flag to indicate that the active thread has timed out.
// (Only the active thread will check this flag.)
volatile int active_thread_timed_out = 0;

void *process1(void *args)
{
    int thread_id = *((int *)args);
    // Let threads start a little later
    sleep(1);
    int attempts = 1;

    while (1)
    {
        // If this thread is not active and a timeout has been signaled,
        // we want it to continue trying normally.
        printf("Thread %d is attempting to access the resource.\n", thread_id);
        int lock_status = pthread_mutex_trylock(&mutex);
        if (lock_status == 0)
        {
            // We acquired the mutex, so this thread is now the active thread.
            attempts = 1;
            printf("||---Thread %d has gained access to the resource.---||\n", thread_id);
            start_time = time(NULL);
            active_thread = pthread_self();
            active_thread_id = thread_id;
            active_thread_timed_out = 0;  // Reset flag upon acquiring resource

            printf("Thread %d write: writing...\n", thread_id);
            // Simulate work (sleep for a random time between 1 and 10 seconds)
            sleep(rand() % 10 + 1);
            shared_resource = rand() % 10 + 1;

            // Check if this active thread has been signaled to stop
            if (active_thread_timed_out && active_thread_id == thread_id) {
                printf("Thread %d detected timeout, releasing resource and exiting.\n", thread_id);
                pthread_mutex_unlock(&mutex);
                break;
            }

            printf("||---Thread %d has released the resource.---||\n", thread_id);
            pthread_mutex_unlock(&mutex);
            sleep(1);
        }
        else
        {
            printf("Thread %d is unable to access the resource. Attempt %d\n", thread_id, attempts);
            sleep(1);
            attempts++;
        }
    }

    printf("Thread %d was stopped, exiting.\n", thread_id);
    return NULL;
}

void *watchdog(void *args)
{
    // The watchdog continuously checks how long the active thread has held the mutex.
    while (1)
    {
        sleep(1);
        time_t current_time = time(NULL);
        long elapsed = current_time - start_time;
        printf("Watchdog: Resource has been in use for %ld seconds.\n", elapsed);

        // If the active thread has held the resource for 3 seconds or more, signal a timeout.
        if (elapsed >= 3 && active_thread_timed_out == 0)
        {
            printf(" ~ Watchdog: Timeout for thread %d. Requesting release. ~ \n", active_thread_id);
            active_thread_timed_out = 1;
            // We don't forcefully unlock the mutex here;
            // instead, the active thread will check the flag and exit after finishing its work.
        }
    }
    return NULL;
}

int main()
{
    pthread_t id1, id2, id3;
    int thread_id1 = 1, thread_id2 = 2, thread_id3 = 3;

    // Create two worker threads.
    pthread_create(&id1, NULL, process1, &thread_id1);
    pthread_create(&id2, NULL, process1, &thread_id2);
    // Create the watchdog thread.
    pthread_create(&id3, NULL, watchdog, &thread_id3);

    // Wait for the worker threads to exit.
    pthread_join(id1, NULL);
    pthread_join(id2, NULL);

    // In this example, the watchdog runs in an infinite loop.
    // Once the workers have exited, we can cancel it.
    pthread_cancel(id3);
    pthread_join(id3, NULL);

    pthread_mutex_destroy(&mutex);

    printf("All threads have exited. Program terminating.\n");
    return 0;
}
