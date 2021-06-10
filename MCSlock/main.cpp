#include <unistd.h>
#include <iostream>
#include <thread>
#include <atomic>
#include <cassert>

/* Data types */
struct QNode
{
	std::atomic<QNode *> 	next;
	std::atomic<bool>	wait;
};

class MCSLock
{
public:
	MCSLock();
	void acquire();
	void release();

private:
	std::atomic<QNode *> 	_tail;
};

/* Shared variables */
const uint32_t		NUM_OF_THREADS = std::thread::hardware_concurrency();
MCSLock			lock;
uint32_t		shared_data;

/* Private variables */
thread_local QNode	q;

/* Child thread's code */
void thread_entry(uint32_t value)
{
        lock.acquire();

        shared_data = value;

        lock.release();
}

/* Main thread's code */
int main()
{
	uint32_t 	i;
	std::thread 	threads[NUM_OF_THREADS];

	for (i = 0; i < NUM_OF_THREADS; ++i)
		threads[i] = std::thread(thread_entry, i);

	for (i = 0; i < NUM_OF_THREADS; ++i)
		threads[i].join();

	std::cout << "shared_data = " << shared_data << std::endl;

	return 0;
}

MCSLock::MCSLock()
{
	_tail = nullptr;
}

/* Mine */
void MCSLock::acquire()
{
	q.next.store(nullptr);
	QNode *prev = _tail.exchange(&q);

	if (prev != nullptr)
	{
		q.wait.store(true);
		prev->next.store(&q);

		while (q.wait.load() == true);
	}
}

void MCSLock::release()
{
	if (q.next.load() == nullptr)
	{
		QNode *p = &q;	//Why?!
		if (_tail.compare_exchange_strong(p, nullptr))
			return;

		while (q.next.load() == nullptr);
	}

	q.next.load()->wait.store(false);
}
/**/

/* Yours *//*
void MCSLock::acquire(QNode *p) {
    p->next=nullptr;
    p->wait=true;

    QNode* prev = _tail.exchange(p);
    if( prev!=nullptr ) {
      prev->next = p;
      while( p->wait.load() ) { } // spin
    }
    //std::atomic_thread_fence(std::memory_order_seq_cst);
  }

  void MCSLock::release(QNode* p) {
    //std::atomic_thread_fence(std::memory_order_release);
    volatile QNode *succ = p->next;
    if( succ==nullptr ) {
      if ( _tail.compare_exchange_strong(p, nullptr) )
        return;

      while( !(succ=p->next) ) {
        printf("waiting %p\n", _tail.load());
      }

    }
    printf("%p\n", succ); assert(succ);
    succ->wait = false; // release next
  }
/**/
