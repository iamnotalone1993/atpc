#include <iostream>	// std::cout...
#include <thread>	// std::thread...
#include <atomic>	// std::atomic...
#include <chrono>	// std::chrono...
#include <cassert>	// std::assert...

/* Shared variables */
const int		NUM_OF_THREADS = 2;
std::atomic<int>	flag{0}, val{0};

/* Private variables */
// thread_local int	a;

/* Child thread's code */
void thread_entry(uint32_t my_tid)
{
	if (my_tid == 0)
	{
		//std::this_thread::sleep_for(std::chrono::milliseconds(100));
		val.store(42, std::memory_order_relaxed);
		flag.store(1, std::memory_order_relaxed);
	}
	else if (my_tid == 1)
	{
		while (flag.load(std::memory_order_relaxed) != 1);
		int tmp = val.load(std::memory_order_relaxed);
		std::cout << "val = " << tmp << std::endl;
		assert(val == 42);
	}
	else { /* do nothing */ }
}

/* Main thread's code */
int main()
{
	uint32_t 	i;
	std::thread 	threads[NUM_OF_THREADS];

	for (i = 0; i < NUM_OF_THREADS; ++i)
		threads[i] = std::thread(thread_entry, i);

	for (i = 0; i < NUM_OF_THREADS; ++i)
		threads[i].join();

	return 0;
}

