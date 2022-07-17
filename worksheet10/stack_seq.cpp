#include <iostream>	// std::cout...
#include <chrono>	// std::chrono...
#include <cmath>	// exp2...
#include <cassert>	// assert...
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
};

/* Stack's Implementation */
template <typename T>
Stack<T>::Stack() : top {nullptr} {}

template <typename T>
void Stack<T>::push(Node<T> *n)
{
	n->next = top;
	top = n;
}

template <typename T>
Node<T> *Stack<T>::pop()
{
	if (top == nullptr)
		return nullptr;
	Node<T>* res = top;
	top = top->next;
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
	assert(top == nullptr);
}

/* Shared variables */
Stack<int>	MyStack;

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
		MyStack.push(n);
		n = MyStack.pop();
		// delete n;
	}

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
