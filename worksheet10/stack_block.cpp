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
	Node<T>*	top;
	std::mutex	mtx;
};

/* Stack's Implementation */
template <typename T>
Stack<T>::Stack() : top {nullptr} {}

template <typename T>
void Stack<T>::push(Node<T> *n)
{
	mtx.lock();	// acquire the lock
	n->next = top;
	top = n;
	mtx.unlock();	// release the lock
}

template <typename T>
Node<T> *Stack<T>::pop()
{
	mtx.lock();	// acquire the lock
	if (top == nullptr)
	{
		mtx.unlock();	// release the lock
		return nullptr;
	}
	Node<T>* res = top;
	top = top->next;
	mtx.unlock();	// release the lock
	return res;
}

template <typename T>
void Stack<T>::print()
{
	std::cout << "Printing the stack..." << std::endl;
	for (Node<T>* tmp = top; tmp != nullptr; tmp = tmp->next)
		std::cout << tmp->val << std::endl;
	std::cout << "*********************" << std::endl;
}

template <typename T>
void Stack<T>::test()
{
	/* Do nothing */
}

/* Shared variables */
Stack<int>	MyStack;

/* Private variables */
// thread_local ...

/* Child thread's code */
inline void thread_entry(int tid, int num_ops)
{
	// Producer Consumer
	Node<int> *n;
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
	Node<int>* n;
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
