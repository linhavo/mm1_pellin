/*
 * Log.h
 *
 *  Created on: 19.1.2013
 *      Author: neneko
 */

#ifndef LOG_H_
#define LOG_H_

#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include "iimaudio/AudioTypes.h"

namespace iimaudio {

class NullBuf: public std::streambuf
{
public:
	NullBuf() {}
};
class NullLogger: public std::ostream {
public:
	NullLogger():std::ostream(&nb) {}
	~NullLogger() {}
template<typename T>
	std::ostream &operator<<(T &)
	{
		return *this;
	}
private:
	NullBuf nb;
};

enum class log_level {
	fatal 	= 0,
	info	= 1,
	debug	= 2
};
class Log {
private:
	static log_level mode;
	NullLogger null;
	std::ostream &stream;
public:
	Log(std::ostream& stream):stream(stream) {}
	void set_global_mode(log_level mode_) {
		mode = mode_;
		//(*this)[log_level::info] << "[Log] Log level set to " << mode_ << std::endl;
	}
	std::ostream &operator[](log_level level_) {
		if (!active(level_)) return null;
		else return stream;
	}
	inline bool active(log_level level) {
		return level<=mode;
	}
};
extern Log logger;


/*!
 * @brief Ancillary class for initializing std::map
 *
 * Usage std::map<A, B> name_of_map = InitMap<A, B>(a0, b0)(a1, b1)(a2, b2);
 */
template<class Key, class Value> class InitMap {
public:
	operator std::map<Key, Value>() { return tmp_map; }
	InitMap() {}
	InitMap(const Key& key, const Value& value) {tmp_map[key]=value;}
	InitMap & operator() (const Key& key, const Value& value) {tmp_map[key]=value;return *this;}
protected:
	std::map<Key, Value> tmp_map;
};

template<class Value> class InitVector {
public:
	operator std::vector<Value>() { return tmp_vec; }
	InitVector() {}
	InitVector(const Value& val) { tmp_vec.push_back(val); }
	InitVector & operator() (const Value& val) {tmp_vec.push_back(val); return *this;}
protected:
	std::vector<Value> tmp_vec;
};


}



#endif /* LOG_H_ */
