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
	T				val;
	std::atomic<CountedPtr<T>>	next;
};

template <typename T>
struct CountedPtr
{
	Node<T>		*ptr;
	unsigned	count;
};

/* Queue's Interface */
template <typename T>
class Queue
{
public:
	Queue();
	void enq(Node<T> *n);
	Node<T> *deq();
	void print();
	void test();

private:
	std::atomic<CountedPtr<T>>	head;
	std::atomic<CountedPtr<T>>	tail;
};

/* Queue's Implementation */
template <typename T>
Queue<T>::Queue()
{
	Node<T>*	init = new Node<T>;
	CountedPtr<T>	tmp = init->next.load();
	init->next = {nullptr, tmp.count};
	head = tail = {init, 0};
}

template <typename T>
void Queue<T>::enq(Node<T> *n)
{
	CountedPtr<T>	oldtail,
			next,
			tmp;
	backoff		bk(DELAY_MIN, DELAY_MAX);

	while (true)					// Keep trying until Enqueue is done
	{
		oldtail = tail.load();			// Read tail.ptr and tail.count together
		next = oldtail.ptr->next;		// Read next ptr and count fields together
		tmp = tail.load();
		if (oldtail.ptr == tmp.ptr
			&& oldtail.count == tmp.count)	// Are tail and next consistent?
		{
			if (next.ptr == nullptr)	// Was tail pointing to the last node?
			{
				// Try to link node at the end of the linked list
				if (oldtail.ptr->next.compare_exchange_weak(next, {n, next.count + 1}))
						break;	// Enqueue is done. Exit loop
				// bk.delay();
			}
			else				// tail was not pointing to the last node
			{	// Try to swing tail to the next node
				if (!tail.compare_exchange_weak(oldtail, {next.ptr, oldtail.count + 1}))
				{
					// bk.delay();
				}
			}
		}
	}
	// Enqueue is done. Try to swing tail to the inserted node
	tail.compare_exchange_weak(oldtail, {n, oldtail.count + 1});
}

template <typename T>
Node<T> *Queue<T>::deq()
{
	CountedPtr<T>	oldhead,
			oldtail,
			next,
			tmp;
	Node<T>*	res;
	backoff		bk(DELAY_MIN, DELAY_MAX);

	while (true)						// Keep trying until Dequeue is done
	{
		oldhead = head.load();				// Read head
		oldtail = tail.load();				// Read tail
		next = head.load().ptr->next;			// Read head.ptr->next
		tmp = head.load();
		if (oldhead.ptr == tmp.ptr
			&& oldhead.count == tmp.count)		// Are head, tail, and next consistent?
		{
			if (head.load().ptr == tail.load().ptr)	// Is queue empty or tail falling behind?
			{
				if (next.ptr == nullptr)	// Is queue empty?
					return nullptr;		// Queue is empty, couldn't dequeue
				// Tail is falling behind. Try to advance it
				if (!tail.compare_exchange_weak(oldtail, {next.ptr, oldtail.count + 1}))
				{
					bk.delay();
				}
			}
			else 					// No need to deal with tail
			{
				// Read value before CAS, otherwise another dequeue might free the next node
				res = next.ptr;
				// Try to swing head to the next node
				if (head.compare_exchange_weak(oldhead, {next.ptr, oldhead.count + 1}))
					return res;		// Dequeue is done. Exit loop
				// bk.delay();
			}
		}
	}
}

template <typename T>
void Queue<T>::print()
{
	std::cout << "Printing the queue..." << std::endl;
	for (CountedPtr<T> tmp = head; head.ptr != tail.ptr; tmp = (tmp.ptr)->next)
		std::cout << (tmp.ptr)->val << std::endl;
	std::cout << "*********************" << std::endl;
	std::cout << "head.count = " << head.load().count << std::endl;
	std::cout << "tail.count = " << tail.load().count << std::endl;
}

template <typename T>
void Queue<T>::test()
{
        // assert(top.load().count == TOTAL_OPS / 2);
}

/* Shared variables */
Queue<int>	MyQueue;

/* Private variables */
// thread_local ...

/* Child thread's code */
inline void thread_entry(int tid, int num_ops)
{
	Node<int>	*n;
	CountedPtr<int>	tmp;

	// Producer Consumer
	for (auto i = 0; i < num_ops; ++i)
	{
		if (tid % 2 == 0)
		{
			n = new Node<int>;
			n->val = i;
			tmp = n->next.load();
			n->next = {nullptr, tmp.count};
			MyQueue.enq(n);
		}
		else // if (tid % 2 != 0)
		{
			n = MyQueue.deq();
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
	CountedPtr<int>	tmp;
	for (int i = 0; i < TOTAL_OPS / 2; ++i)
	{
		n = new Node<int>;
		n->val = i;
		tmp = n->next.load();
		n->next = {nullptr, tmp.count};
		MyQueue.enq(n);
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
	// MyQueue.print();

	// Test the stack
	// MyQueue.test();

	return 0;
}
