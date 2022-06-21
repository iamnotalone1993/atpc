#include <atomic>	// std::atomic...

/* compare_exchange_WEAK implementation */
template<typename T>
bool atomic_compare_exchange_weak(	std::atomic<T>*	obj,
					T* 		expected,
					T		val	)
{
	T tmp = LL(obj);	// LL: Load Linked
	if (tmp != *expected)	// indicate a REGULAR failure
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
bool atomic_compare_exchang_strong(	std::atomic<T>*	obj,
					T*		expected,
					T		val	)
{
	T tmp;
	while (true)
	{
		tmp = LL(obj);		// LL: Load Linked
		if (tmp != *expected)	// indicate a REGULAR failure
		{
			*expected = tmp;
			return false;
		}
	
		if (SC(obj, val))	// SC: Store Conditional
			return true;	// indicate a success
	}
}

