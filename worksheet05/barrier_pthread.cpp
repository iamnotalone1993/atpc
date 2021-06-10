#include <iostream>	// std::cout...
#include <pthread.h>	// pthread_t...
#include <chrono>	// std::chrono...
#include "config.h"	// configurations...

/* Shared variables */
pthread_barrier_t	barrier;

/* Child thread's code */
void *thread_entry(void *ptr)
{
	for (auto i = 0; i < EPOCHS; ++i)
        	pthread_barrier_wait(&barrier);
	
	return nullptr;
}

/* Main thread's code */
int main()
{
	using namespace std::chrono;

	pthread_t 	threads[NUM_OF_THREADS];

	pthread_barrier_init(&barrier, nullptr, NUM_OF_THREADS);

	std::cout << "Pthread Barrier\n";

        auto start = high_resolution_clock::now();

	for (auto i = 0; i < NUM_OF_THREADS; ++i)
		pthread_create(&threads[i], nullptr, thread_entry, nullptr);

	for (auto i = 0; i < NUM_OF_THREADS; ++i)
		pthread_join(threads[i], nullptr);

        auto stop = high_resolution_clock::now();

        auto duration = duration_cast<milliseconds>(stop - start);

	std::cout << double(EPOCHS) / duration.count() << " episodes/ms\n";

	return 0;
}

