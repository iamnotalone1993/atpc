#include <iostream>	// std::cout...
#include <thread>	// std::thread...
#include <mutex>	// std::mutex...
#include "config.h"	// TOTAL_OPS...

template <typename T>
struct Node 
{
	T		val;
	Node<T>*	next;
};

/* Queue's Interface */
template <typename T>
class Queue
{
public:
	Queue();
	void enqueue(Node<T> *n);
	Node<T> *dequeue();
	void print();
	void test();

private:
	Node<T>*	head;
	Node<T>*	tail;
	std::mutex	mtx;
};

/* Queue's Implementation */
template <typename T>
Queue<T>::Queue() : head {nullptr}, tail {nullptr} {}

template <typename T>
void Queue<T>::enqueue(Node<T> *n)
{
	mtx.lock();	// acquire the lock
	if (tail == nullptr)
		head = tail = n;
	else // if (tail != nullptr)
	{
		tail->next = n;
		tail = n;
	}
	mtx.unlock();	// release the lock
}

template <typename T>
Node<T> *Queue<T>::dequeue()
{
	mtx.lock();	// acquire the lock
	if (head == nullptr)
	{
		mtx.unlock();	// release the lock
		return nullptr;
	}
	Node<T>* res = head;
	head = head->next;
	if (head == nullptr)
		tail = nullptr;
	mtx.unlock();	// release the lock
	return res;
}

template <typename T>
void Queue<T>::print()
{
	std::cout << "Printing the queue..." << std::endl;
	for (Node<T>* tmp = head; tmp != nullptr; tmp = tmp->next)
		std::cout << tmp->val << std::endl;
	std::cout << "*********************" << std::endl;
}

template <typename T>
void Queue<T>::test()
{
	/* Do nothing */
}

/* Shared variables */
Queue<int>	MyQueue;

/* Private variables */
// thread_local ...

/* Child thread's code */
inline void thread_entry(int tid, int num_ops)
{
	// Producer Consumer
	Node<int>	*n;
	for (auto i = 0; i < num_ops; ++i)
	{
		if (tid % 2 == 0)
		{
			n = new Node<int>;
			*n = {i, nullptr};
			MyQueue.enqueue(n);
		}
		else // if (tid % 2 != 0)
		{
			n = MyQueue.dequeue();
			// delete n;
		}
	}
}

/* Main thread's code */
int main()
{
	using namespace std::chrono;

	std::thread 	threads[NUM_THREADS];

	int num_ops = TOTAL_OPS / NUM_THREADS;

	// Warm up
	Node<int>* n;
	for (int i = 0; i < TOTAL_OPS / 2; ++i)
	{
		n = new Node<int>;
		*n = {i, nullptr};
		MyQueue.enqueue(n);
	}

	// Start timer
	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	// The main thread forks
	for (auto tid = 0; tid < NUM_THREADS; ++tid)
		threads[tid] = std::thread(thread_entry, tid, num_ops);

	// The child threads join
	for (auto tid = 0; tid < NUM_THREADS; ++tid)
		threads[tid].join();

	// Stop timer
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	duration<double> time_span = duration_cast<duration<double>>(t2 - t1);

	std::cout << "The throughput = " << TOTAL_OPS / time_span.count() / 1000
		<< " operations per ms" << std::endl;

	// Print the queue
	// MyQueue.print();

	// Test the queue
	// MyQueue.test();

	return 0;
}
