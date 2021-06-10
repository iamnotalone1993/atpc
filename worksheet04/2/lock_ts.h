#ifndef LOCK_TS_H
#define	LOCK_TS_H

#include <iostream>	// std::cout...
#include <atomic>	// std::atomic_flag...
#include "config.h"	// configurations
#include "backoff.h"	// backoff

/* Interfaces */

class lock_ts
{
public:
	lock_ts();
	void acquire();
	void release();

private:
	std::atomic_flag	f;
};

class lock_ts_exp
{
public:
	lock_ts_exp();
	void acquire();
	void release();

private:
	std::atomic_flag	f;
};

/* Implementations */

lock_ts::lock_ts() : f(ATOMIC_FLAG_INIT)
{
        std::cout << "Test-and-Set Lock" << std::endl;
}

void lock_ts::acquire()
{
        while (f.test_and_set());
}

void lock_ts::release()
{
        f.clear();
}

lock_ts_exp::lock_ts_exp() : f(ATOMIC_FLAG_INIT)
{
        std::cout << "Test-and-Set Lock + Exponential Backoff" << std::endl;
}

void lock_ts_exp::acquire()
{
        backoff bk(DELAY_MIN, DELAY_MAX);

        while (f.test_and_set())
                bk.delay();
}

void lock_ts_exp::release()
{
        f.clear();
}

#endif /* LOCK_TS_H */
