#include <iostream>	// std::cout...
#include <chrono>	// std::chrono...
#include <cstdint>	// uint64_t...
#include <omp.h>	// #pragma omp...
#include "config.h"	// configurations...

int main(int argc, char *argv[])
{
	using namespace std::chrono;

	std::cout << "OpenMP Barrier\n";

        auto start = high_resolution_clock::now();

	#pragma omp parallel num_threads(NUM_OF_THREADS)
	{
		for (auto i = 0; i < EPOCHS; ++i)
		{
			#pragma omp barrier
		}
	}
	
        auto stop = high_resolution_clock::now();

        auto duration = duration_cast<milliseconds>(stop - start);

	std::cout << double(EPOCHS) / duration.count() << " episodes/ms" << std::endl;

	return 0;
}

