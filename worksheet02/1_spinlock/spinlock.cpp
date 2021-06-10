#include <iostream>	// std::cout...
#include <thread>	// std::thread...
#include <atomic>	// std::atomic...
#include <cstdint>	// uint32_t...

/* Spinlock implementation */
class spinlock
{
        std::atomic<bool> flag;

public:
        spinlock() : flag(false) {}	// CLEAR = false, SET = true

        void lock()
        {
                bool expected;

                do {
                        expected = false; // CAUTION!!!: This variable can be changed to TRUE after executing the next c_e_w
                } while (!flag.compare_exchange_weak(expected, true, std::memory_order_acquire));
		// Attempt to change the flag from CLEAR to SET
        }

        void unlock()
        {
                flag.store(false, std::memory_order_release);	// Clear the flag
        }
};

/* Shared variables */
const uint32_t		NUM_OF_THREADS = std::thread::hardware_concurrency();
spinlock		sl;
uint32_t		shared_data;

/* Private variables */
// thread_local int	a;

/* Child thread's code */
void thread_entry(uint32_t value)
{
        sl.lock();

        shared_data = value;

	std::cout << "shared_data = " << shared_data << std::endl;

        sl.unlock();
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

