#include <iostream>	// std::cout...
#include <thread>	// std::thread...
#include <atomic>	// std::atomic...
#include <cstdint>	// uint64_t...
#include <cassert>	// std::assert...
#include <array>	// std::array...
#include "config.h"	// configurations...

/* Barrier implementation */
class barrier_sr
{
public:
	barrier_sr() : count{0}, sense{true} {}

	void arrive_and_wait()
	{
		lsense = !lsense;			// toggle local sense
		if (count++ == NUM_OF_THREADS - 1)
		{
			count = 0;			// prepare for next episode
			sense = lsense;			// release threads
		}
		else
			while (sense != lsense);	// spin
	}

private:
	std::atomic<uint64_t> 		count;
	std::atomic<bool>		sense;
	static thread_local bool	lsense;
};

/* Shared variables */
barrier_sr barrier;

/* Private variables */
thread_local bool barrier_sr::lsense = true;

/* Child thread's code */
void thread_entry()
{
	for (auto i = 0; i < EPOCHS; ++i)
        	barrier.arrive_and_wait();
}

/* Main thread's code */
int main()
{
	using namespace std::chrono;

	std::thread 	threads[NUM_OF_THREADS];

	std::cout << "Sense Reversing Barrier\n";

	auto start = high_resolution_clock::now();

	for (auto i = 0; i < NUM_OF_THREADS; ++i)
		threads[i] = std::thread(thread_entry);

	for (auto i = 0; i < NUM_OF_THREADS; ++i)
		threads[i].join();

        auto stop = high_resolution_clock::now();

        auto duration = duration_cast<milliseconds>(stop - start);

        std::cout << double(EPOCHS) / duration.count() << " episodes/ms" << std::endl;

	return 0;
}

