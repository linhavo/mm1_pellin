/*
 * AudioPolicies.h
 *
 *  Created on: 20.1.2013
 *      Author: neneko
 */

#ifndef AUDIOPOLICIES_H_
#define AUDIOPOLICIES_H_
#include <mutex>
namespace iimaudio {
/*
struct Lock {
	Lock(pthread_mutex_t *mutex):mutex(mutex) {
		pthread_mutex_lock(mutex);
	}
	Lock(pthread_mutex_t &mutex):mutex(&mutex) {
		pthread_mutex_lock(this->mutex);
	}
	Lock(Lock &&other) {
		mutex=other.mutex;
		other.mutex=0;
	}
	~Lock() {
		if (mutex) pthread_mutex_unlock(mutex);
	}
protected:
	pthread_mutex_t *mutex;
};*/

class SingleThreaded {
public:
	SingleThreaded() {}
	virtual ~SingleThreaded() {}

	typedef bool lock_t; // Dummy lock type
	lock_t lock_instance() { return true; }
};

class MultiThreaded {
public:
	MultiThreaded() {}
	virtual ~MultiThreaded() {}

	typedef std::unique_lock<std::mutex> lock_t;
	lock_t lock_instance() { return lock_t(native_lock_); }
private:
	std::mutex native_lock_;
};

}



#endif /* AUDIOPOLICIES_H_ */
