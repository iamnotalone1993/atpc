#ifndef LOCK_CXX_H
#define	LOCK_CXX_H

#include <iostream>	// std::cout...
#include <mutex>	// std::mutex...

/* Interfaces */

class lock_cxx
{
public:
	lock_cxx();
	void acquire();
	void release();

private:
	std::mutex	mtx;
};

/* Implementations */

lock_cxx::lock_cxx()
{
        std::cout << "C++ Lock" << std::endl;
}

void lock_cxx::acquire()
{
        mtx.lock();
}

void lock_cxx::release()
{
        mtx.unlock();
}

#endif /* LOCK_CXX_H */
