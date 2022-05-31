#include <atomic>	// std::atomic...

/* compare_exchange_WEAK implementation */
template<typename T>
bool atomic_compare_exchange_weak(	std::atomic<T>*	obj,
					T* 		expected,
					T		val	)
{
	T tmp = LL(obj);	// LL: Load Linked
	if (tmp != *expected)	// a REGULAR failure
	{
		*expected = tmp;
		return false;
	}

	if (SC(obj, val))	// SC: Store Conditional
		return true;	// indicate a success
	return false;		// indicate a SPURIOUS failure
}

/* compare_exchange_STRONG implementation */
template<typename T>
bool atomic_compare_exchange_strong(	std::atomic<T>*	obj,
					T*		expected,
					T		val	)
{
	bool	res;
	T 	exp_prev = *expected;	// save the original content of parameter @expected
	
	do {
		res = atomic_compare_exchange_weak_explicit(obj, expected, val, success, failure);
	} while (!res && exp_prev == *expected);	// retry if encountering a spurious failure

	return res;
}

