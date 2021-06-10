#include <iostream>	// std::cout...
#include <thread>	// std::thread...
#include <chrono>	// std::chrono...
#include "config.h"	// configurations
#include "lock.h"	// locks: ts, tts, mcs, c++

bool is_prime(uint64_t p)
{
	for (uint64_t d = 2; d < p; ++d)
		if (p % d == 0)
			return false;
	return true;
}

/* Shared variables */
lock_mcs	lock;

/* Private variables */
// thread_local int dummy

/* Child thread's code */
void thread_entry()
{
	for (auto i = 0; i < NUM_OF_ITERS; ++i)
	{
        	lock.acquire();

		// check if a number N is a prime
		is_prime(N);

        	lock.release();

		for (auto j = 0; j < OUTSIDE_WORK; ++j)
			is_prime(N);
	}
}

/* Main thread's code */
int main()
{
	using namespace std::chrono;

	std::thread 	threads[NUM_OF_THREADS];

        auto start = high_resolution_clock::now();

	for (auto i = 0; i < NUM_OF_THREADS; ++i)
		threads[i] = std::thread(thread_entry);

	for (auto i = 0; i < NUM_OF_THREADS; ++i)
		threads[i].join();

        auto stop = high_resolution_clock::now();

        auto duration = duration_cast<microseconds>(stop - start);

        std::cout << duration.count()  << " microseconds" << std::endl;

	return 0;
}

