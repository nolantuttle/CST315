#include <stdio.h>
#include <pthread.h>

int theProduct;

int get()
{
	return 1;
}

void put(int i)
{
	i += 1;
}

int produce()
{
	return theProduct++;
}

void consume(int i)
{
	printf("%i", i);
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

int main()
{

	return 0;
}
