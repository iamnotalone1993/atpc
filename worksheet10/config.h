#ifndef CONFIG_H
#define CONFIG_H

#include <cstdint>	// uint64_t...
#include <cmath>	// exp2...

const uint64_t	DELAY_MIN	=	0;		// exp2(DELAY_MIN) nanoseconds
const uint64_t 	DELAY_MAX	=	10;		// exp2(DELAY_MAX) nanoseconds
const uint64_t	NUM_THREADS	=	8;		// number of running threads in the program
const uint64_t	TOTAL_OPS	=	exp2(20);	// total number of operations executed by all threads

#endif /* CONFIG_H */
