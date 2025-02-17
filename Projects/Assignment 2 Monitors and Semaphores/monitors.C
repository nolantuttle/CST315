/*Written by Mathew Hobson*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

typedef struct Monitor
{
    int readers; //Number of active readers
    int writing; //Number of active writers
}Monitor;

//Initialize the monitor
void initMonitor(Monitor *m){

    m->readers=0;
    m->writing =0;

}

void startRead(Monitor *m){
    if(m->writing){
        printf("[Reader BLOCKED] A writer is writing\n");
    }
    m->readers++;
    printf("[Reader Start], %d readers are reading\n",m->readers);
}
void endRead(Monitor *m){
    if(m->readers==0)return;
    m->readers--;
    printf("[Reader END] %d reader(s) remain.\n", m->readers);
    
}
// Start writing (Writer Entry)
void startWrite(Monitor *m) {
    if (m->writing || m->readers > 0) {
        printf("[Writer BLOCKED] Readers active or another writer is writing.\n");
        return;
    }
    m->writing = 1;
    printf("[Writer START] Writer is writing.\n");
}

// End writing (Writer Exit)
void endWrite(Monitor *m) {
    if (!m->writing) return; // Ensure only active writers call this
    m->writing = 0;
    printf("[Writer END] Writing finished. Readers/Writers can proceed.\n");
}

int main() {

    // Creating shared memory to initialize the monitor
    int memID = shmget(IPC_PRIVATE, sizeof(Monitor), IPC_CREAT | 0666);
    Monitor *monitor = (Monitor *)shmat(memID, NULL, 0);
    initMonitor(monitor);

    int totalReaders = 5;
    int totalWriters = 3;

    // Forking reader processes
    for (int i = 0; i < totalReaders; i++) {
        pid_t pid = fork();
        if (pid == 0) { // Child process (reader)
            startRead(monitor);  // Reader starts
            sleep(2);            // Simulate reading
            endRead(monitor);    // Reader ends
            exit(0);             // Exit child process
        }
    }

    // Forking writer processes (these writers will be blocked)
    for (int i = 0; i < totalWriters; i++) {
        pid_t pid = fork();
        if (pid == 0) { // Child process (writer)
            startWrite(monitor);  // Writer starts
            sleep(3);             // Simulate writing
            endWrite(monitor);    // Writer ends
            exit(0);              // Exit child process
        }
    }

    // Wait for readers to be done
    sleep(5);

    // Forking writer processes (this writer will not be blocked)
    pid_t pid = fork();
    if (pid == 0) { // Child process (writer)
            startWrite(monitor);  // Writer starts
            sleep(1);             // Simulate writing
            endWrite(monitor);    // Writer ends
            exit(0);              // Exit child process
        }

    // Wait for all child processes to finish
    for (int i = 0; i < 5 + 3; i++) {
        wait(NULL);
    }

    // Clean up shared memory
    shmdt(monitor);
    shmctl(memID, IPC_RMID, NULL);
    
    return 0;
}