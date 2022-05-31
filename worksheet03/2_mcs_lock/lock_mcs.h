#ifndef LOCK_MCS_H
#define LOCK_MCS_H

#include <iostream>	// std::cout...
#include <atomic>	// std::atomic...

/* Interfaces */

struct QNode
{
        std::atomic<QNode *>    next;
        std::atomic<bool>       wait;
};

class lock_mcs
{
public:
        lock_mcs();
        void acquire();
        void release();

private:
        std::atomic<QNode *>    tail;
};

thread_local QNode      q;

lock_mcs::lock_mcs() : tail{nullptr} {}

void lock_mcs::acquire()
{
        q.next.store(nullptr);
        QNode *prev = tail.exchange(&q);

        if (prev != nullptr)
        {
                q.wait.store(true);
                prev->next.store(&q);

                while (q.wait.load() == true);
        }
}

void lock_mcs::release()
{
        if (q.next.load() == nullptr)
        {
                QNode *p = &q;
                if (tail.compare_exchange_strong(p, nullptr))
                        return;

                while (q.next.load() == nullptr);
        }
        q.next.load()->wait.store(false);
}

#endif /* LOCK_MCS_H */
