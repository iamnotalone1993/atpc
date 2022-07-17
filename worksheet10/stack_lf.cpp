#include <iostream>	// std::cout...
#include <thread>	// std::thread...
#include <atomic>	// std::atomic...
#include <chrono>	// std::chrono...
#include <cmath>	// exp2...
#include <cassert>	// assert...
#include "config.h"	// TOTAL_OPS...
#include "backoff.h"	// backoff...

template <typename T> struct CountedPtr;

template <typename T>
struct Node 
{
	T		val;
	CountedPtr<T>	next;
};

template <typename T>
struct CountedPtr
{
	Node<T>		*ptr;
	unsigned	count;
};

/* Stack's Interface */
template <typename T>
class Stack
{
public:
	Stack();
	void push(Node<T> *n);
	Node<T> *pop();
	void print();
	void test();

private:
	std::atomic<CountedPtr<T>>	top;
};

/* Stack's Implementation */
template <typename T>
Stack<T>::Stack() : top {{nullptr, 0}} {}

template <typename T>
void Stack<T>::push(Node<T> *n)
{
	CountedPtr<T>	oldtop,
			newtop;
	backoff		bk(DELAY_MIN, DELAY_MAX);

	while (true)
	{
		oldtop = top.load();
		n->next = oldtop;
		newtop = {n, oldtop.count};
		if (top.compare_exchange_weak(oldtop, newtop))
			return;
		bk.delay();
	}
}

template <typename T>
Node<T> *Stack<T>::pop()
{
	CountedPtr<T>	oldtop,
			newtop;
	backoff		bk(DELAY_MIN, DELAY_MAX);

	while (true)
	{
		oldtop = top.load();
		if (oldtop.ptr == nullptr)
			return nullptr;
		newtop = (oldtop.ptr)->next;
		newtop.count = oldtop.count + 1;
		if (top.compare_exchange_weak(oldtop, newtop))
			return oldtop.ptr;
		bk.delay();
	}
}

template <typename T>
void Stack<T>::print()
{
	std::cout << "Printing the stack..." << std::endl;
	for (CountedPtr<T> tmp = top; tmp.ptr != nullptr; tmp = (tmp.ptr)->next)
		std::cout << (tmp.ptr)->val << std::endl;
	std::cout << "*********************" << std::endl;
	std::cout << "top.count = " << top.load().count << std::endl;
}

template <typename T>
void Stack<T>::test()
{
        assert(top.load().count == TOTAL_OPS / 2);
}

/* Shared variables */
Stack<int>	MyStack;

/* Private variables */
// thread_local ...

/* Child thread's code */
inline void thread_entry(int tid, int num_ops)
{
	Node<int>	*n;

	// Producer Consumer
	for (auto i = 0; i < num_ops; ++i)
	{
		if (tid % 2 == 0)
		{
			n = new Node<int>;
			*n = {i, nullptr};
			MyStack.push(n);
		}
		else // if (tid % 2 != 0)
		{
			n = MyStack.pop();
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
	Node<int>*	n;
	for (int i = 0; i < TOTAL_OPS / 2; ++i)
	{
		n = new Node<int>;
		*n = {i, nullptr};
		MyStack.push(n);
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

	// Print the stack
	// MyStack.print();

	// Test the stack
	// MyStack.test();

	return 0;
}
