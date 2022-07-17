#include <atomic>	// std::atomic...

/* CASConsensus */
template<typename T>
class MMSConsensus
{
private:	
	const int		FIRST = -1;
	std::atomic<int>	reg;
	std::atomic<int>	*array;
	T			*proposal;

public:
	// initialize register
	MMSConsensus()
		:reg{FIRST},
		array{new std::atomic<int>[NUM_THREADS]},
		proposal{new T[NUM_THREADS]} 
	{
		for (int tid = 0; tid < NUM_THREADS; ++tid)
			array[tid] = tid;
	}

	// figure out which thread was first
	T decide(T val)
	{
		proposal[self] = val;	
		MMS(array[self], reg);	// MMS: atomic mem-mem swap
		for (int tid = 0; tid < NUM_THREADS; ++tid)
			if (array[tid] == FIRST)
				return proposal[tid];
	}
};

