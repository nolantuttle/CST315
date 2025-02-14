#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>

// Declaring semaphores
sem_t mutex;
sem_t writerSemaphore;
sem_t readerSemaphore;

// This is the shared resource being read/written to
int resource = 0;

int activeReaders = 0;

void *reader(void *args)
{
    // Current semaphore will wait until the resource is unlocked
    sem_wait(&mutex);

    activeReaders++;
    if (activeReaders == 1) {
        // If this is the first reader, wait for the writer
        sem_wait(&writerSemaphore);
    }

    // Signals the mutex to unlock
    sem_post(&mutex);

    // Simulate reading the shared resource
    printf("Reader %ld is reading. Shared resource: %d\n", pthread_self(), resource);

    // Waits to simulate the read execution
    sleep(2);

    // Waits for mutex before removing a reader
    sem_wait(&mutex);

    activeReaders--;
    // If there are no active readers, signal the writer semaphore
    if (activeReaders == 0) {
        sem_post(&writerSemaphore);
    }

    // Unlocks the mutex (binary semaphore)
    sem_post(&mutex);

    return NULL;
}

void *writer(void *args)
{
    // Sleep so that console output looks better
    // Since everything runs concurrently, the output is different each time without this delay
    sleep(2);
    
    // Writer semaphore will wait for signal
    sem_wait(&writerSemaphore);

    // Write to shared resource
    resource++;

    // Print statement to show resource being updated
    printf("Writer %ld is writing\n", pthread_self());
    printf("Done writing value %d\n", resource);

    // Waits to simulate write execution
    sleep(2);

    // Signals that the semaphore is done writing
    sem_post(&writerSemaphore);

    return NULL;
}

int main()
{    
    int totalReaders = 5;
    int totalWriters = 3;

    pthread_t readers[totalReaders];
    pthread_t writers[totalWriters];

    // Initializing semaphores
    sem_init(&mutex, 0, 1);            // Mutex initialized to 1 (binary semaphore)
    sem_init(&writerSemaphore, 0, 1);   // Write semaphore to allow only one writer
    sem_init(&readerSemaphore, 0, 1);    // Semaphore to control access for readers

    // Creates 5 reader threads
    for(int i = 0; i < totalReaders; i++)
    {
        pthread_create(&readers[i], NULL, reader, NULL);
    }

    // Creates 5 writer threads
    for(int i = 0; i < totalWriters; i++)
    {
        pthread_create(&writers[i], NULL, writer, NULL);
    }

    // Makes sure we wait for all threads to finish
    for(int i = 0; i < totalReaders; i++)
    {
        pthread_join(readers[i], NULL);
    }
    for(int i = 0; i < totalWriters; i++)
    {
        pthread_join(writers[i], NULL);
    }

    // Destroy allocated resources
    sem_destroy(&mutex);
    sem_destroy(&writerSemaphore);
    sem_destroy(&readerSemaphore);

return 0;
}
