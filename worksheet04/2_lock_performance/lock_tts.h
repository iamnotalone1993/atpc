#ifndef LOCK_TTS_H
#define	LOCK_TTS_H

#include <iostream>	// std::cout...
#include <atomic>	// std::atomic...
#include "config.h"	// configurations
#include "backoff.h"	// backoff

/* Interfaces */

class lock_tts
{
public:
	lock_tts();
	void acquire();
	void release();

private:
	std::atomic<bool>	f;
};

class lock_tts_exp
{
public:
	lock_tts_exp();
	void acquire();
	void release();

private:
	std::atomic<bool>	f;
};

/* Implementations */

lock_tts::lock_tts() : f(ATOMIC_FLAG_INIT)
{
        std::cout << "Test and Test-and-Set Lock" << std::endl;
}

void lock_tts::acquire()
{
        while (f.exchange(true))
                while (f) {}
}

void lock_tts::release()
{
        f = ATOMIC_FLAG_INIT;
}

lock_tts_exp::lock_tts_exp() : f(ATOMIC_FLAG_INIT)
{
        std::cout << "Test and Test-and-Set Lock + Exponential Backoff" << std::endl;
}

void lock_tts_exp::acquire()
{
        backoff bk(DELAY_MIN, DELAY_MAX);

        while (f.exchange(true))
                while (f)
                        bk.delay();
}

void lock_tts_exp::release()
{
        f = ATOMIC_FLAG_INIT;
}

#endif /* LOCK_TTS_H */
