#ifndef LOCK_MCS_H
#define LOCK_MCS_H

#include <iostream>	// std::cout...
#include <atomic>	// std::atomic...
#include "config.h"	// configurations
#include "backoff.h"	// backoff

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

class lock_mcs_exp
{
public:
        lock_mcs_exp();
        void acquire();
        void release();

private:
        std::atomic<QNode *>    tail;
};

thread_local QNode      q;

lock_mcs::lock_mcs() : tail(nullptr)
{
        std::cout << "MCS Lock" << std::endl;
}

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

lock_mcs_exp::lock_mcs_exp() : tail(nullptr)
{
        std::cout << "MCS Lock + Exponential Backoff" << std::endl;
}

void lock_mcs_exp::acquire()
{
        backoff bk(DELAY_MIN, DELAY_MAX);

        q.next.store(nullptr);
        QNode *prev = tail.exchange(&q);

        if (prev != nullptr)
        {
                q.wait.store(true);
                prev->next.store(&q);

                while (q.wait.load() == true)
                        bk.delay();
        }
}

void lock_mcs_exp::release()
{
        backoff bk(DELAY_MIN, DELAY_MAX);

        if (q.next.load() == nullptr)
        {
                QNode *p = &q;
                if (tail.compare_exchange_strong(p, nullptr))
                        return;

                while (q.next.load() == nullptr)
                        bk.delay();
        }
        q.next.load()->wait.store(false);
}


#endif /* LOCK_MCS_H */
