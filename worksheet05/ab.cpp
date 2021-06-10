#include <thread>	// std::thread...
#include <atomic>	// std::atomic...
#include <cstdint>	// uint64_t...
#include <cassert>	// std::assert...
#include <array>	// std::array...

/* Constants */
const uint64_t NUM_OF_THREADS	= std::thread::hardware_concurrency();
const uint64_t EPOCHS		= 1000;

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
barrier_sr					barrier;
std::array<std::atomic<uint64_t>, EPOCHS>	shared_data;

/* Private variables */
thread_local bool barrier_sr::lsense = true;

/* Child thread's code */
void thread_entry()
{
	for (auto i = 0; i < EPOCHS; ++i)
	{
		++shared_data[i];

        	barrier.arrive_and_wait();

		assert(shared_data[i] == NUM_OF_THREADS);
	}
}

/* Main thread's code */
int main()
{
	std::thread 	threads[NUM_OF_THREADS];

	std::fill(shared_data.begin(), shared_data.end(), 0);

	for (auto i = 0; i < NUM_OF_THREADS; ++i)
		threads[i] = std::thread(thread_entry);

	for (auto i = 0; i < NUM_OF_THREADS; ++i)
		threads[i].join();

	return 0;
}

