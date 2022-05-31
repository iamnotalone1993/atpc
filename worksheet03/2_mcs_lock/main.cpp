#include <iostream>	// std::cout...
#include <thread>	// std::thread...
#include <atomic>	// std::atomic...
#include "lock_mcs.h"	// lock_mcs...

/* Shared variables */
const int	NUM_THREADS	= std::thread::hardware_concurrency();
const int	NUM_ITERS	= 10000;
lock_mcs	MCSlock;
int		counter;

/* Child thread's code */
void thread_entry(int tid)
{
	for (int i = 0; i < NUM_ITERS; ++i)
	{
        	MCSlock.acquire();	// acquire the mutex lock
        	++counter;		// increment the counter
       		MCSlock.release(); 	// release the mutex lock
	}
}

/* Main thread's code */
int main()
{
	int 		i;
	std::thread 	threads[NUM_THREADS];

	// fork
	for (i = 0; i < NUM_THREADS; ++i)
		threads[i] = std::thread(thread_entry, i);

	// join
	for (i = 0; i < NUM_THREADS; ++i)
		threads[i].join();

	// print the incorrect result
	if (counter != NUM_THREADS * NUM_ITERS)
		std::cout << "counter = " << counter << std::endl;

	return 0;
}

