#include <stdio.h>
#include <pthread.h>

int theProduct;

int produce()
{
	return theProduct++;
}

void consume(int i)
{
	printf("%i", i);
}

int get()
{
	return produce();
}

void put(int i)
{
	consume(i);
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
		consume(i);
	}
}

void *busy(void *ptr)
{
	producer();
	return NULL;
}

void *busy1(void *ptr)
{
	consumer();
	return NULL;
}

int main()
{

	pthread_t id;
	pthread_create(&id, NULL, busy, "Producer"); // this one produces

	pthread_create(&id, NULL, busy1, "Consumer"); // consume!

	while (1)
	{
	}

	return 0; // you get 0 back
}
