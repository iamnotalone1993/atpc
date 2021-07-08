#include <iostream>
#include <thread>
#include <atomic>

template <typename T>
struct Node 
{
	T	val;
	Node	*next;
};

/* Shared Variables */
const uint64_t  		NUM_OF_THREADS  = std::thread::hardware_concurrency();
std::atomic<Node<int> *>	*HP;	// Array of Hazard Pointers
/**/

/* Private variables */
thread_local uint64_t		TID;	// Thread's ID
/**/

/* Stack's Interface */
template <typename T>
class Stack
{
public:
	Stack();
	bool push(const T &val);
	bool pop(T &val);
	void print();

private:
	std::atomic<Node<T> *>	top;
};

/* Stack's Implementation */
template <typename T> Stack<T>::Stack()
{
	top = nullptr;
}

template <typename T>
bool Stack<T>::push(const T &val)
{
	Node<T> *oldtop, *newtop;

	newtop = new Node<T>;
	if (newtop == nullptr)
		return false;

	do {
		oldtop = top.load();
		*newtop = {val, oldtop};
	} while (!top.compare_exchange_weak(oldtop, newtop));

	return true;
}

template <typename T>
bool Stack<T>::pop(T &val)
{
	Node<T> *oldtop, *newtop;

	do {
		oldtop = top.load();
		if (oldtop == nullptr)
			return false;

                /* HPs */
                HP[TID] = oldtop;
                if (oldtop != top.load())
                        continue;
                /**/

		newtop = oldtop->next;
	} while (!top.compare_exchange_weak(oldtop, newtop));

	/* HPs */
	HP[TID] = nullptr;
	/**/

	val = oldtop->val;

	/* HPs */
	bool flag = true;
	for (uint64_t i = 0; i < NUM_OF_THREADS; ++i)
		if (oldtop == HP[i])
		{
			flag = false;
			break;
		}
	if (flag == true)
		delete oldtop;
	else //if (flag == false)
		{ /* Memory Leaks */ }
	/**/

	return true;
}

template <typename T>
void Stack<T>::print()
{
	std::cout << "Printing the stack..." << std::endl;
	for (Node<T> *tmp = top; tmp != nullptr; tmp = tmp->next)
		std::cout << tmp->val << std::endl;
	std::cout << "*********************" << std::endl;
}

/* Shared variables */
Stack<int>	MyStack;

/* Child thread's code */
inline void thread_entry(uint64_t tid)
{
	int	val;

	TID = tid;

	//Sequential Alternating
	for (uint64_t i = 0; i < 10; ++i)
	{
		val = i;
		MyStack.push(val);
		//MyStack.pop(val);
	}
}

/* Main thread's code */
int main()
{
	uint64_t 	i;
	std::thread 	threads[NUM_OF_THREADS];

	// Allocate memory for and initialize the array of Hazard Pointers
        HP = new std::atomic<Node<int> *>[NUM_OF_THREADS];
	for (i = 0; i < NUM_OF_THREADS; ++i)
		HP[i] = nullptr;

	// The main thread forks
	for (i = 0; i < NUM_OF_THREADS; ++i)
		threads[i] = std::thread(thread_entry, i);

	// The child threads join
	for (i = 0; i < NUM_OF_THREADS; ++i)
		threads[i].join();

	// Deallocate memory of the array of Hazard Pointers
	delete HP;

	// Print the stack
	MyStack.print();

	return 0;
}
