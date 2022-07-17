#include <atomic>	// std::atomic...
#include <thread>	// std::thread...
#include <cassert>	// assert...

/* Private variables */
thread_local int 	self;

/* Shared variables */
const int		NUM_THREADS = 3;

template<typename T>
class FAAConsensus
{
private:
	const int		FIRST = 0;
	const int 		AMOUNT = 5; // AMOUNT > 0
	std::atomic<int>	reg;
	T			*proposal;

public:
	// initialize register
	FAAConsensus(): reg{FIRST}, proposal{new T[NUM_THREADS]} {}

	// figure out which thread was first
	T decide(T val)
	{
		proposal[self] = val;
		if (reg.fetch_add(AMOUNT) == FIRST)
			return proposal[self];
		else // if (reg.fetch_add(1) != FIRST)
			return proposal[1-self];
	}
};

/* Shared variables */
FAAConsensus<int>	consensus;
int			*array;

/* Child thread's code */
void thread_entry(int tid)
{
	self = tid;	// get my thread identifier
	array[self] = consensus.decide(self);
}

/* Main thread's code */
int main()
{
	int 		i;
	std::thread 	threads[NUM_THREADS];
	
	array = new int[NUM_THREADS];

	// fork
	for (i = 0; i < NUM_THREADS; ++i)
		threads[i] = std::thread(thread_entry, i);

	// join
	for (i = 0; i < NUM_THREADS; ++i)
		threads[i].join();

	// check if the consensus protocol is working correctly
	for (i = 1; i < NUM_THREADS; ++i)
		assert(array[0] == array[i]);

	return 0;
}
