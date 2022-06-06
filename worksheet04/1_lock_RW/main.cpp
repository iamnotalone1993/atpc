#include <iostream>	// std::cout...
#include <thread>	// std::thread...
#include <shared_mutex>	// std::shared_mutex...
#include <cstdint>      // uint32_t...
#include <chrono>	// std::chrono::seconds...

/* Shared variables & constants */
const uint32_t          NUM_OF_THREADS = 3;	// thread count
const bool		READER = true;		// a reader
const bool		WRITER = false;		// a writer
std::shared_mutex	mutex;			// readers-writer lock
uint32_t                shared_data = 0;	// shared data

/* Private variables */
//thread_local int     tmp;

/* Child thread's code */
void thread_entry(uint32_t thread_id, bool is_reader)
{
	if (is_reader)	// calling thread is a reader
	{	
		mutex.lock_shared();	// reader_acquire()

        	std::cout << "[" << thread_id << "]shared_data = " << shared_data << std::endl;	// read data

		while (true);	// loop forever

		mutex.unlock_shared();	// reader_release()
	}
	else	// calling thread is a writer
	{
		mutex.lock();	// writer_acquire()

		shared_data = 1;	// write data

		std::cout << "[" << thread_id << "]shared_data = " << shared_data << std::endl;	// read data

		while (true);	// loop forever

		mutex.unlock();	// writer_release()
	}
}

/* Main thread's code */
int main()
{
	std::thread 	threads[NUM_OF_THREADS];	// an array of child threads (a declaration)

	threads[0] = std::thread(thread_entry, 0, WRITER);	// spawn child thread 0

	std::this_thread::sleep_for(std::chrono::seconds(1));	// sleep for 1 sec

	threads[1] = std::thread(thread_entry, 1, READER);	// spawn child thread 1

	std::this_thread::sleep_for(std::chrono::seconds(1));   // sleep for 1 sec

	threads[2] = std::thread(thread_entry, 2, READER);	// spawn child thread 2

	for (auto i = 0; i < NUM_OF_THREADS; ++i)	// all child threads join here and then terminate
		threads[i].join();

	return 0;
}

