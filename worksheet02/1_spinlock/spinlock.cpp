#include <iostream>	// std::cout...
#include <thread>	// std::thread...
#include <atomic>	// std::atomic...
#include <cstdint>	// uint32_t...

/* Spinlock implementation */
class spinlock
{
private:
        std::atomic<bool>	flag;
	const bool		CLEAR	= false;
	const bool		SET	= true;

public:
        spinlock() : flag(CLEAR) {}

        void lock()
        {
                bool expected;

                do {
                        expected = CLEAR; 	// CAUTION!!!: 	This variable can be changed to
						// 		SET after executing the next c_e_w
                } while (!flag.compare_exchange_weak(expected, SET, std::memory_order_acquire));
		// attempt to change the flag from CLEAR to SET
        }

        void unlock()
        {
                flag.store(CLEAR, std::memory_order_release);	// clear the flag
        }
};

/* Shared variables */
const uint32_t		NUM_THREADS = std::thread::hardware_concurrency();
spinlock		mutex;
uint32_t		shared_data;

/* Private variables */
// thread_local int	a;

/* Child thread's code */
void thread_entry(uint32_t tid)
{
        mutex.lock();	// acquire the mutex lock

        shared_data = tid;

	std::cout << "shared_data = " << shared_data << std::endl;

       	mutex.unlock(); // release the mutex lock
}

/* Main thread's code */
int main()
{
	uint32_t 	i;
	std::thread 	threads[NUM_THREADS];

	// fork
	for (i = 0; i < NUM_THREADS; ++i)
		threads[i] = std::thread(thread_entry, i);

	// join
	for (i = 0; i < NUM_THREADS; ++i)
		threads[i].join();

	return 0;
}

