#ifndef CONFIG_H
#define CONFIG_H

#include <cstdint>	// uint64_t...

const uint64_t	NUM_OF_THREADS	= 	8;
const uint64_t	NUM_OF_ITERS	= 	100000;
const uint64_t	OUTSIDE_WORK 	= 	2;	
const uint64_t	N		= 	7919;	// the 1,000th prime
const uint64_t  DELAY_MIN       =	0;      // exp2(DELAY_MIN) nanoseconds
const uint64_t  DELAY_MAX       =       10;     // exp2(DELAY_MAX) nanoseconds


#endif /* CONFIG_H */
