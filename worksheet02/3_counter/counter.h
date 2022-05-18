#ifndef COUNTER_H
#define COUNTER_H

#include <atomic>	// std::atomic...
#include <cstdint>	// uint64_t...
#include "config.h"	// MIN_DELAY, MAX_DELAY
#include "backoff.h"	// backoff...

class counter
{
public:
	counter() : val(0) {}
	void incr_ces();	// compare_exchange_strong
	void incr_cew();	// compare_exchange_weak
	void incr_ces_bk();	// compare_exchange_strong with backoff
	void incr_cew_bk();	// compare_exchange_weak with backoff
	void incr_fa();		// fetch_add

private:
	std::atomic<uint64_t>	val;
};

void counter::incr_ces()
{
	uint64_t 	val_old = val.load(),
	    		val_new;

	do {
		val_new = val_old + 1;
	} while (!val.compare_exchange_strong(val_old, val_new));
}

void counter::incr_cew()
{
	uint64_t	val_old = val.load(),
			val_new;

	do {
		val_new = val_old + 1;
	} while (!val.compare_exchange_weak(val_old, val_new));
}

void counter::incr_ces_bk()
{
	uint64_t 	val_old,
	    		val_new;
	backoff		bk(DELAY_MIN, DELAY_MAX);

	while (true)
	{
		val_old = val.load();
		val_new = val_old + 1;
		if (val.compare_exchange_strong(val_old, val_new))
			return;
		else // if (!val.compare_exchange_strong(val_old, val_new))
			bk.delay();
	}
}

void counter::incr_cew_bk()
{
	uint64_t	val_old,
			val_new;
	backoff		bk(DELAY_MIN, DELAY_MAX);

	while (true)
	{
		val_old = val.load();
		val_new = val_old + 1;
                if (val.compare_exchange_strong(val_old, val_new))
                        return;
		else // if (!val.compare_exchange_weak(val_old, val_new))
                	bk.delay();
	}
}

void counter::incr_fa()
{
	val.fetch_add(uint64_t(1));
}

#endif /* COUNTER_H */
