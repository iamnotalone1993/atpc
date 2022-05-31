#include <iostream>	// std::cout...
#include <thread>	// std::thread...
#include <atomic>	// std::atomic...
#include <cassert>	// assert...

/* Private variables */
thread_local int        self;

/* Peterson lock implementation */
class PetersonLock
{
private:
	std::atomic<int>	turn;
        std::atomic<bool>	interested[2];

public:
        PetersonLock()
	{
		for (auto &x : interested)
			x = false;
	}

        void acquire()
        {
                int other = 1 - self;
		interested[self] = true;	// I'm interested
		turn = other;			// you go first

		while (interested[other] && turn == other);	// spin
        }

        void release()
        {
                interested[self] = false;
        }
};

/* Shared variables */
const int	NUM_THREADS	= 2;
const int	NUM_ITERS	= 10000;
PetersonLock	Plock;
int		counter;

/* Child thread's code */
void thread_entry(int tid)
{
	self = tid;		// get my thread identifier

	for (int i = 0; i < NUM_ITERS; ++i)
	{
        	Plock.acquire();	// acquire the mutex lock
        	++counter;		// increment the counter
       		Plock.release(); 	// release the mutex lock
	}
}

/* Main thread's code */
int main()
{
	int 		i;
	std::thread 	threads[NUM_THREADS];

	// fork
	for (i = 0; i < NUM_THREADS; ++i)
		threads[i] = std::thread(thread_entry, i);

	// join
	for (i = 0; i < NUM_THREADS; ++i)
		threads[i].join();

	// print the incorrect result
	if (counter != NUM_THREADS * NUM_ITERS)
		std::cout << "counter = " << counter << std::endl;

	return 0;
}

