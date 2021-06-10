#ifndef BACKOFF_H
#define BACKOFF_H

#include <thread>	// std::this_thread::sleep_for...
#include <chrono>	// std::chrono::nanoseconds...
#include <cmath>	// exp2...
#include <cstdint>	// uint64_t...

class backoff
{
public:
	backoff(const uint64_t &mi, const uint64_t &ma) : min(mi), max(ma) {}

	void delay()	// exponential backoff
	{
        	std::this_thread::sleep_for(std::chrono::nanoseconds(uint64_t(exp2(min))));
        	if (min < max)
                	++min;
	}

private:
	uint64_t	min;	// min delay
	uint64_t	max;	// max delay
};

#endif /* BACKOFF_H */
