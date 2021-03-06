#include <limits>	// std::numeric_limits...
#include <iostream>	// std::cout...
#include <thread>	// std::thread...
#include <mutex>	// std::mutex...
#include <chrono>	// std::chrono...
#include <random>	// std::uniform_int_distribution...

using namespace std::chrono;

template<typename T>
class Node
{
public:
	T val;
	Node<T> *next = nullptr;

	Node(const T& v) : val(v) {}
	Node(const T& v, const Node<T> *p) : val(v), next(p) {}
};

template<typename T>
class Set
{
	Node<T> 	tail;
	Node<T> 	head;
	std::mutex	mtx;

public:
	Node<T>* find_node(const T& val);

	bool add(const T& val);
	bool remove(const T& val);
	bool contains(const T& val);

	void dump();
  
public:
	Set() : head(std::numeric_limits<T>::min()),
		tail(std::numeric_limits<T>::max())
	{
		head.next = &tail;
	}
};

template<typename T>
void Set<T>::dump()
{
	Node<T> *node = &head;
	while (node) 
	{
		std::cout << node << " " << node->val << "\n";
		node = node->next;
	}
	std::cout << "------------------------\n\n";
}

template<typename T>
Node<T>* Set<T>::find_node(const T& val)
{
	Node<T> *curr = &head;
	Node<T> *next = curr->next;
  
	while (next && next->val < val)
	{
		curr = next;
		next = curr->next;
	}
	return curr;
}

template<typename T>
bool Set<T>::add(const T& val)
{
	// acquire the mutex lock
	mtx.lock();

	Node<T> *res = find_node(val);
	if (res->next->val == val)
	{
		// release the mutex lock
		mtx.unlock();

		return false;
	}

	Node<T> *node = new Node<T>(val);
	node->next = res->next;
	res->next = node;

	// release the mutex lock
	mtx.unlock();

	return true;
}

template<typename T>
bool Set<T>::remove(const T& val)
{
	// acquire the mutex lock
	mtx.lock();

	Node<T> *res = find_node(val);
	if (res->next->val != val)
	{
		// release the mutex lock
		mtx.unlock();

		return false;
	}

	Node<T> *node = res->next;
	res->next = node->next;
	delete(node);

	// release the mutex lock
	mtx.unlock();

	return true;
}

template<typename T>
bool Set<T>::contains(const T& val)
{
	// acquire the mutex lock
	mtx.lock();

	Node<T> *res = find_node(val);
	T tmp = res->next->val;

	// release the mutex lock
	mtx.unlock();

	return tmp == val;
}

/* Shared variables & constants */
const int	NUM_THREADS	= 8;		// thread count
const int 	NUM_OPS		= 10000;	// operation count per thread
Set<int>	s;

/* Child thread's code */
void thread_entry(int tid)
{
	int num, code;
	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(0, NUM_THREADS * NUM_OPS);

	for (int i = 0; i < NUM_OPS; ++i)
	{
		num = distribution(generator);	// generate number in the range 0...NUM_THREADS x NUM_OPS
		code = num % 3;
		if (code == 0)
			s.add(num);
		else if (code == 1)
			s.remove(num);
		else // if (code == 2)
			s.contains(num);
	}
}

/* Main thread's code */
int main(int argc, char* artv[])
{
	std::thread	threads[NUM_THREADS];	// an array of child threads (a declaration)

	auto start = high_resolution_clock::now();	// start timing

	for (auto tid = 0; tid < NUM_THREADS; ++tid)
		threads[tid] = std::thread(thread_entry, tid);	// spawn child threads

	for (auto tid = 0; tid < NUM_THREADS; ++tid)	// all child threads join here and then terminate
		threads[tid].join();

	auto stop = high_resolution_clock::now();	// stop timing
	auto duration = duration_cast<milliseconds>(stop - start);
	std::cout << double(NUM_THREADS * NUM_OPS) / duration.count() << " operations per millisecond" << std::endl;

	return 0;
}
