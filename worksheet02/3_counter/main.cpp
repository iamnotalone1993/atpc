#include <iostream>	// std::cout...
#include <thread>	// std::thread...
#include <chrono>	// std::chrono...
#include <cstdint>	// uint64_t...
#include "config.h"	// NUM_OF_THREADS, NUM_OF_OPS
#include "counter.h"	// counter

/* Shared variables */
counter	mycounter;

/* Private variables */
//thread_local uint64_t	test;

/* Child thread's code */
void thread_entry()
{
	for (uint64_t i = 0; i < NUM_OF_OPS; ++i)
		mycounter.incr_cew_bk();
}

/* Main thread's code */
int main()
{
	using namespace std::chrono;

	uint64_t 	i;
	std::thread 	threads[NUM_OF_THREADS];

	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	for (i = 0; i < NUM_OF_THREADS; ++i)
		threads[i] = std::thread(thread_entry);

	for (i = 0; i < NUM_OF_THREADS; ++i)
		threads[i].join();

	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	duration<double> time_span = duration_cast<duration<double>>(t2 - t1);

	//std::cout << "The total execution time = " << time_span.count() << " seconds" << std::endl;
	std::cout << "The throughput = " << NUM_OF_THREADS * NUM_OF_OPS / time_span.count() / 1000
			<< " operations per ms" << std::endl;

	return 0;
}
