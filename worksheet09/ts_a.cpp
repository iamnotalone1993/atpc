#include <iostream>
#include <thread>
#include <atomic>

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

private:
	std::atomic<CountedPtr<T>>	top;
};

/* Stack's Implementation */
template <typename T>
Stack<T>::Stack()
{
	top = {nullptr, 0};
}

template <typename T>
void Stack<T>::push(Node<T> *n)
{
	CountedPtr<T> oldtop, newtop;

	do {
		oldtop = top.load();
		n->next = oldtop;
		newtop = {n, oldtop.count};
	} while (!top.compare_exchange_weak(oldtop, newtop));
}

template <typename T>
Node<T> *Stack<T>::pop()
{
	CountedPtr<T> oldtop, newtop;

	do {
		oldtop = top.load();
		if (oldtop.ptr == nullptr)
			return nullptr;
		newtop = (oldtop.ptr)->next;
		++(newtop.count);
	} while (!top.compare_exchange_weak(oldtop, newtop));

	return oldtop.ptr;
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

/* Shared variables */
const uint64_t	NUM_OF_THREADS	= std::thread::hardware_concurrency();
Stack<int>	MyStack;

/* Private variables */
//thread_local ...

/* Child thread's code */
inline void thread_entry()
{
	Node<int>	*n;

	//Sequential Alternating
	for (uint64_t i = 0; i < 10; ++i)
	{
		n = new Node<int>;
		n->val = i;
		MyStack.push(n);
		//n = MyStack.pop();
		//delete n;
	}
}

/* Main thread's code */
int main()
{
	uint64_t 	i;
	std::thread 	threads[NUM_OF_THREADS];

	// Check if CounterPtr is lock-free
	std::atomic<CountedPtr<int>>	cptr;
	if (cptr.is_lock_free())
		std::cout << "CountedPtr is lock-free!" << std::endl;
	else
		std::cout << "CountedPtr is not lock-free!" << std::endl;

	// The main thread forks
	for (i = 0; i < NUM_OF_THREADS; ++i)
		threads[i] = std::thread(thread_entry);

	// The child threads join
	for (i = 0; i < NUM_OF_THREADS; ++i)
		threads[i].join();

	// Print the stack
	MyStack.print();

	return 0;
}
