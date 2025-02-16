/*Written by Mathew Hobson*/
#include<stdio.h>



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
    printf("[Reader Start], %d readers started reading\n",m->readers);
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

// Monitor Cleanup (Optional)
void destroyMonitor(Monitor *m) {
    // No resources to free, but placeholder for future extensions
}

int main() {
    Monitor monitor;
    initMonitor(&monitor);

    // Simulating function calls in a sequential execution
    startRead(&monitor);
    startRead(&monitor);
    endRead(&monitor);
    startWrite(&monitor);
    startRead(&monitor);  // This should be blocked
    endWrite(&monitor);
    startRead(&monitor);
    endRead(&monitor);
    destroyMonitor(&monitor);

    return 0;
}