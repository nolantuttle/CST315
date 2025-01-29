#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#define STACK_SIZE 5 // Define the size of the integer buffer
int theProduct;

// mutex variable
pthread_mutex_t stack_mutex = PTHREAD_MUTEX_INITIALIZER;

// structure to represent a stack
struct Stack
{
    int stack[STACK_SIZE]; // array of elements
    int bottom;            // pointer to bottom of the stack
    int top;               // pointer to top of the stack
    int count;             // counter
};

// initialize the stack with 0s with the current element being 0 and count being 0
static struct Stack stack = {{0, 0, 0, 0, 0}, 0, 0};

// Function to check if the stack is empty
bool isEmpty(struct Stack stack)
{
    return stack.count == 0;
}

// Function to check if the stack is full
bool isFull(struct Stack stack)
{
    return stack.count == STACK_SIZE;
}

// Function to add an element to the stack (push)
void push(int value)
{
    // lock the mutex before accessing stack so other thread can't access it
    pthread_mutex_lock(&stack_mutex);

    if (isFull(stack))
    {
        printf("Stack is full.\n");
        // unlocks the mutex if stack is full
        pthread_mutex_unlock(&stack_mutex);
        return;
    }
    stack.stack[stack.top] = value;
    printf("Produced %d in slot %i.\n", value, stack.top);
    stack.top = (stack.top + 1) % STACK_SIZE; // brings the current element back to the start when it reaches last index
    stack.count++;
    // unlocks mutex if push was successfull
    pthread_mutex_unlock(&stack_mutex);
}

// function to pop/consume an element from the stack
int consume(int i)
{
    // lock the mutex before accessing stack so other thread can't access it
    pthread_mutex_lock(&stack_mutex);
    if (isEmpty(stack))
    {
        printf("Stack is empty.\n");
        // unlocks the mutex if stack is empty
        pthread_mutex_unlock(&stack_mutex);
        return -1; // Return an invalid value to indicate failure
    }

    // starts from the bottom of the stack (FIFO)
    int value = stack.stack[stack.bottom];
    printf("Consumed %i from slot %i \n", value, stack.bottom);

    stack.bottom = (stack.bottom + 1) % STACK_SIZE;
    stack.count--;

    pthread_mutex_unlock(&stack_mutex);
    return value;
}

int produce()
{ // locks mutex before modifying product count (it is shared between both threads)
    pthread_mutex_lock(&stack_mutex);
    int product = theProduct++;
    pthread_mutex_unlock(&stack_mutex);
    return product;
}

void put(int i)
{
    while (isFull(stack))
    {
        sleep(5);
    }
    push(i);
}

int get()
{
    while (isEmpty(stack))
    {
        sleep(5);
    }
    return consume(stack.count);
}

void producer()
{
    int i;
    while (1)
    {
        i = produce();
        put(i);
    }
}
void consumer()
{
    int i;
    while (1)
    {
        i = get();
    }
}

void *consumer1(void *args)
{
    consumer();
    return NULL;
}

void *producer1(void *args)
{
    producer();
    return NULL;
}

int main()
{

    pthread_t id1, id2;
    pthread_create(&id1, NULL, consumer1, "Consumer"); // this one produces

    pthread_create(&id2, NULL, producer1, "Producer"); // consume!

    pthread_join(id1, NULL);
    pthread_join(id2, NULL);

    // delete mutex after done
    pthread_mutex_destroy(&stack_mutex);

    return 0;
}