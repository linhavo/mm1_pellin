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

class SingleThreaded {
public:
	SingleThreaded() {}
	virtual ~SingleThreaded() {}

	typedef bool lock_t; // Dummy lock type
	lock_t lock_instance() const { return true; }
};

class MultiThreaded {
public:
	MultiThreaded() {}
	virtual ~MultiThreaded() {}

	typedef ::std::unique_lock<std::mutex> lock_t;
	lock_t lock_instance() const { return lock_t(native_lock_); }
private:
	mutable ::std::mutex native_lock_;
};

}



#endif /* AUDIOPOLICIES_H_ */
