#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>   // For O_CREAT flag
#include <sys/types.h>
#include <unistd.h>  // For fork()
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

sem_t *resource_semaphore;
#define NUM_PROCESSES 5
#define RESOURCE_TIMEOUT 8
time_t bruh[NUM_PROCESSES];

pid_t pids[NUM_PROCESSES];


void *resource1(void* arg){
    sem_wait(resource_semaphore);
    printf("Resource entered...\n");
    sleep(3);
    printf("Exiting...\n");
    sem_post(resource_semaphore);



}
void *resource2(void* arg){
    sem_wait(resource_semaphore);
    printf("Resource entered...\n");
    sleep(3);
    printf("Exiting...\n");
    sem_post(resource_semaphore);

}
void restart_process(int index) {
    printf("Process %d is being restarted...\n", pids[index]);

    // Kill the starved process
    kill(pids[index], SIGKILL);
    waitpid(pids[index], NULL, 0); // Wait for termination

    // Fork a new process to replace the killed one
    pids[index] = fork();
    if (pids[index] < 0) {
        perror("Fork failed");
        exit(1);
    }
    else if (pids[index] == 0) { // Child process
        printf("New Process %d created to replace starved process.\n", getpid());
        if (index % 2 == 0) {
            resource1(NULL);
        } else {
            resource2(NULL);
        }
        exit(0);
    }
}

// Check for starvation periodically
void starvation_check(int sig) {
    printf("\n[Starvation Check] Checking for starved processes...\n");

    for (int i = 0; i < NUM_PROCESSES; i++) {
        if (time(NULL) - bruh[i] > RESOURCE_TIMEOUT) {
            restart_process(i);
        }
    }

    alarm(RESOURCE_TIMEOUT);  // Reset starvation timer
}


int main() {
    // Initialize semaphore

    resource_semaphore = sem_open("/resource_semaphore", O_CREAT, 0644, 1);
    if (resource_semaphore == SEM_FAILED) {
        perror("Semaphore initialization failed");
        exit(1);
    }

    printf("Semaphore initialized. Resource is available.\n");

    // Set up starvation check using SIGALRM
    signal(SIGALRM, starvation_check);
    alarm(RESOURCE_TIMEOUT); // Start first starvation timer

    // Create multiple processes
    for (int i = 0; i < NUM_PROCESSES; i++) {
        pids[i] = fork();

        if (pids[i] < 0) {
            perror("Fork failed");
            exit(1);
        }
        else if (pids[i] == 0) { // Child process
            printf("Process %d created.\n", getpid());
            if (i % 2 == 0) {
                bruh[i] = time(NULL);

                resource1(NULL);
            } else {
                bruh[i] = time(NULL);

                resource2(NULL);
            }
            exit(0);
        }
    }

    // Parent waits for processes but also checks for starvation
    while (1) {
        wait(NULL);
    }

    // Cleanup semaphore
    sem_close(resource_semaphore);
    sem_unlink("/resource_semaphore");

    return 0;
}