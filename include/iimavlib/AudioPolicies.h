/**
 * @file 	AudioPolicies.h
 *
 * @date 	20.1.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 * This file defines policies for AudioDevice
 */

#ifndef AUDIOPOLICIES_H_
#define AUDIOPOLICIES_H_
#include <mutex>
namespace iimavlib {

class SingleThreaded {

public:
	SingleThreaded() {}
	virtual ~SingleThreaded() {}

	struct lock_t {~lock_t(){}}; // Dummy lock type. It has dtor to prevent g++4.6 considering it as an unused variable
	lock_t lock_instance() const { return lock_t(); }
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
