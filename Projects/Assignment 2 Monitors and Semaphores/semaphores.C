/*Written by Nolan Tuttle*/
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>

// Declaring semaphores
sem_t *mutex;
sem_t *writerSemaphore;
sem_t *readerSemaphore;

// This is the shared resource being read/written to
int *resource = 0;

// Number of active readers
int *activeReaders = 0;

void reader(int args)
{
    // Current semaphore will wait until the resource is unlocked
    sem_wait(mutex);

    (*activeReaders)++;
    if (*activeReaders == 1) {
        // If this is the first reader, wait for the writer
        sem_wait(writerSemaphore);
    }

    // Signals the mutex to unlock
    sem_post(mutex);

    // Simulate reading the shared resource
    printf("Reader %d is reading. Shared resource value: %d\n", args, *resource);

    // Waits to simulate the read execution
    sleep(2);

    // Waits for mutex before removing a reader
    sem_wait(mutex);

    (*activeReaders)--;
    // If there are no active readers, signal the writer semaphore
    if (*activeReaders == 0) {
        sem_post(writerSemaphore);
    }

    // Unlocks the mutex (binary semaphore)
    sem_post(mutex);
}

void writer(int args)
{
    
    // Writer semaphore will wait for signal
    sem_wait(writerSemaphore);

    // Write to shared resource
    (*resource)++;

    // Print statement to show resource being updated
    printf("Writer %d is writing\n", args);
    printf("Done writing value %d\n", *resource);

    // Waits to simulate write execution
    sleep(2);

    // Signals that the semaphore is done writing
    sem_post(writerSemaphore);
}

int main()
{    
    int totalReaders = 3;
    int totalWriters = 2;

    pthread_t readers[totalReaders];
    pthread_t writers[totalWriters];

    // Creating a shared piece of memory
    // Sets the key, block size (2*sizeof(int)), and flag for shared memory.
    int memID = shmget(IPC_PRIVATE, 2 * sizeof(int), IPC_CREAT | 0666);

    // Attaches shared memory to process
    resource = (int *)shmat(memID, NULL, 0);

    // This should be right after resource in shared memory
    activeReaders = resource + 1;

    // Initializing semaphores
    mutex = sem_open("/mutex", O_CREAT, 0666, 1);  // Mutex semaphore
    writerSemaphore = sem_open("/writerSemaphore", O_CREAT, 0666, 1);  // Writer semaphore
    readerSemaphore = sem_open("/readerSemaphore", O_CREAT, 0666, 1);  // Reader semaphore

    // Forking a single reader process
    pid_t pid = fork();
        if (pid == 0) {
            // Reader process
            reader(1);  // Passes in process number for print stuff
            exit(0);  // Terminate child process after it finishes reading
        }

    // Forking the writer processes
    for (int i = 0; i < totalWriters; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            // Writer process
            writer(i);  // Passes in process number for print stuff
            exit(0);  // Terminate child process after it finishes writing
        }
    }

    // Forking the reader processes
    for (int i = 0; i < totalReaders; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            // Reader process
            reader(i);  // Passes in process number for print stuff
            exit(0);  // Terminate child process after it finishes reading
        }
    }

    // Wait for all child processes to be done
    for (int i = 0; i < totalReaders + totalWriters; i++) {
        wait(NULL);
    }

    // Destroy allocated resources
    sem_close(mutex);
    sem_close(writerSemaphore);
    sem_close(readerSemaphore);

    // Cleanup of shared memory
    shmdt(resource);
    shmctl(memID, IPC_RMID, NULL);

    return 0;
}
