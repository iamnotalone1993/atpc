#include <iostream>	// std::cout...
#include <thread>	// std::thread...
#include <atomic>	// std::atomic...
#include <cstdint>	// uint32_t...

/* compare_exchange_strong implementation */
template<typename T>
bool compare_exchange_strong(	std::atomic<T>*		obj,
				T* 			expected,
				T			val,
				std::memory_order	success,
				std::memory_order	failure	)
					noexcept
{
	bool	res;	// contain the return value of ces
	T	exp_prev = *expected;	// save the original value of parameter @expected

	do {
		res = atomic_compare_exchange_weak_explicit(obj, expected, val, success, failure);
	} while (!res && exp_prev == *expected);	// retry if encountering a spurious failure

	return res;
}

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
                } while (!compare_exchange_strong(&flag, &expected, SET,
						std::memory_order_acquire,
						std::memory_order_acquire));
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

       	mutex.unlock();	// release the mutex lock
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

