#include <iostream>	// std::cout...
#include <chrono>	// std::chrono...
#include <cassert>	// assert...
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
};

/* Queue's Implementation */
template <typename T>
Queue<T>::Queue() : head {nullptr}, tail {nullptr} {}

template <typename T>
void Queue<T>::enqueue(Node<T> *n)
{
	if (tail == nullptr)
		head = tail = n;
	else // if (tail != nullptr)
	{
		tail->next = n;
		tail = n;
	}
}

template <typename T>
Node<T> *Queue<T>::dequeue()
{
	if (head == nullptr)
		return nullptr;
	Node<T>* res = head;
	head = head->next;
	if (head == nullptr)
		tail = nullptr;
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
	assert(head == nullptr);
	assert(tail == nullptr);
}

/* Shared variables */
Queue<int>	MyQueue;

/* Private variables */
// thread_local ...

/* Main thread's code */
int main()
{
	using namespace std::chrono;

	// Start timer
	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	Node<int>*	n;
	for (int i = 0; i < TOTAL_OPS / 2; ++i)
	{
		n = new Node<int>;
		*n = {i, nullptr};
		MyQueue.enqueue(n);
		n = MyQueue.dequeue();
		// delete n;
	}

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
