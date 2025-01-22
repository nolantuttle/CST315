#include <stdio.h>
#include <pthread.h>
#include<unistd.h>
#include<stdbool.h>

#define QUEUE_SIZE 10  // Define the size of the integer buffer

static int queue[QUEUE_SIZE];
static int front = 0;
static int rear = -1;
static int count = 0;
// Function to check if the queue is empty
bool isEmpty() {
    return count == 0;
}

// Function to check if the queue is full
bool isFull() {
    return count == QUEUE_SIZE;
}

// Function to add an element to the queue (enqueue)
void enqueue(int value) {
    if (isFull()) {
        printf("Queue is full. Cannot enqueue %d.\n", value);
        return;
    }
    rear = (rear + 1) % QUEUE_SIZE;  // Circular increment
    queue[rear] = value;
    count++;
    printf("Enqueued %d to the queue.\n", value);
}

// Function to remove an element from the queue (dequeue)
int dequeue() {
    if (isEmpty()) {
        printf("Queue is empty. Cannot dequeue.\n");
        return -1;  // Return an invalid value to indicate failure
    }
    int value = queue[front];
    front = (front + 1) % QUEUE_SIZE;  // Circular increment
    count--;
    printf("Dequeued ");
    return value;
}

int theProduct;
void put(int i){
    while(isFull())
    {
        sleep(1);
    }
    enqueue(i);      
}

int get(){
    while(isEmpty(queue) == true){
        sleep(1);
    }
    return dequeue(queue);

}
int produce(){
    return theProduct++;
}
int consume(int i){
    printf("%i\n", i);
}
void producer() {
    int i;
    while(1){
        i = produce();
        put(i);
        }
    }
void consumer() {
    int i;
    while(1){
        i = get();
        consume(i);
        }
    }

void *consumer1(void *args){
    consumer();
    return NULL;
}

void *producer1(void *args){
    producer();
    return NULL;
}

int main(){
    

    pthread_t id1, id2;
	pthread_create(&id1, NULL, consumer1, "Consumer"); // this one produces

	pthread_create(&id2, NULL, producer1, "Producer"); // consume!
    
    pthread_join(id1, NULL);
	pthread_join(id2,NULL);

}