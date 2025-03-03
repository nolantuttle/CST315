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

// Flags to track whether worker threads are running.
volatile int thread1_running = 0;
volatile int thread2_running = 0;

void *process1(void *args)
{
    int thread_id = *((int *)args);
    // Mark the thread as running.
    if (thread_id == 1)
        thread1_running = 1;
    else if (thread_id == 2)
        thread2_running = 1;

    // Let threads start a little later.
    sleep(1);
    int attempts = 1;

    while (1)
    {
        printf("Thread %d is attempting to access the resource.\n", thread_id);
        int lock_status = pthread_mutex_trylock(&mutex);
        if (lock_status == 0)
        {
            attempts = 1;
            printf("||---Thread %d has gained access to the resource.---||\n", thread_id);
            start_time = time(NULL);
            active_thread = pthread_self();
            active_thread_id = thread_id;
            active_thread_timed_out = 0;  // Reset timeout flag

            printf("Thread %d write: writing...\n", thread_id);
            // Simulate work: sleep for a random time between 1 and 10 seconds.
            sleep(rand() % 10 + 1);
            shared_resource = rand() % 10 + 1;

            // If a timeout was signaled for the active thread, exit.
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
    // Mark the thread as no longer running.
    if (thread_id == 1)
        thread1_running = 0;
    else if (thread_id == 2)
        thread2_running = 0;

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
        }
    }
    return NULL;
}

int main()
{
    pthread_t worker1, worker2, wd;
    int id1 = 1, id2 = 2, wd_id = 3;

    // Start the watchdog thread.
    pthread_create(&wd, NULL, watchdog, &wd_id);

    // Initially create worker threads.
    pthread_create(&worker1, NULL, process1, &id1);
    pthread_create(&worker2, NULL, process1, &id2);

    // Manager loop: continuously monitor and restart threads if they exit.
    while (1)
    {
        sleep(1);
        if (!thread1_running)
        {
            printf("Restarting thread 1.\n");
            pthread_create(&worker1, NULL, process1, &id1);
        }
        if (!thread2_running)
        {
            printf("Restarting thread 2.\n");
            pthread_create(&worker2, NULL, process1, &id2);
        }
    }

    // Cleanup (unreachable in this endless loop, but provided for completeness).
    pthread_cancel(wd);
    pthread_join(wd, NULL);
    pthread_mutex_destroy(&mutex);
    return 0;
}
