#include <iostream>	// std::cout...
#include <thread>	// std::thread...
#include <chrono>	// std::chrono...
#include "config.h"	// configurations
#include "lock.h"	// locks: ts, tts, mcs, c++

using namespace std::chrono;

bool is_prime(uint64_t p)
{
	for (uint64_t d = 2; d < p; ++d)
		if (p % d == 0)
			return false;
	return true;
}

/* Shared variables */
lock_ts	lock;
//microseconds	acc[NUM_OF_THREADS];

/* Private variables */
// thread_local int dummy

/* Child thread's code */
void thread_entry(int tid)
{
	//time_point<high_resolution_clock>	start, stop;
	//acc[tid] = microseconds(0);
	for (auto i = 0; i < NUM_OF_ITERS; ++i)
	{
		// timing
		//start = high_resolution_clock::now();

        	lock.acquire();

		// timing
		//stop = high_resolution_clock::now();
		//acc[tid] += duration_cast<microseconds>(stop - start);

		// check if a number N is a prime
		is_prime(N);

		// timing
		//start = high_resolution_clock::now();

        	lock.release();

		// timing
		//stop = high_resolution_clock::now();
		//acc[tid] += duration_cast<microseconds>(stop - start);

		for (auto j = 0; j < OUTSIDE_WORK; ++j)
			is_prime(N);
	}
}

/* Main thread's code */
int main()
{
	std::thread 	threads[NUM_OF_THREADS];

        auto start = high_resolution_clock::now();

	for (int tid = 0; tid < NUM_OF_THREADS; ++tid)
		threads[tid] = std::thread(thread_entry, tid);

	for (int tid = 0; tid < NUM_OF_THREADS; ++tid)
		threads[tid].join();

        auto stop = high_resolution_clock::now();

        auto duration = duration_cast<microseconds>(stop - start);
	std::cout << duration.count() << " microseconds" << std::endl;

	//for (int tid = 0; tid < NUM_OF_THREADS; ++tid)
        //	std::cout << acc[tid].count()  << " microseconds" << std::endl;

	return 0;
}

