/**
 * @file 	Utils.h
 *
 * @date 	19.1.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 */
#ifndef UTILS_H_
#define UTILS_H_

#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include "iimavlib/AudioTypes.h"
#include "PlatformDefs.h"

namespace iimavlib {
class EXPORT LogProxy {
public:
#ifdef SYSTEM_LINUX
	LogProxy() = delete;
	LogProxy(LogProxy&)=delete;
	LogProxy(const LogProxy&)=delete;
#endif
	LogProxy(std::ostream* str_):stream_(str_){}
	LogProxy(LogProxy&& rhs) throw():stream_(rhs.stream_) {
		const std::string str = rhs.buffer_.str();
		buffer_.write(str.c_str(),str.size());
		rhs.stream_ = nullptr;
	}
	~LogProxy() {
		if (stream_) {
			buffer_  << "\n";
			const std::string str = buffer_.str();
			stream_->write(str.c_str(),str.size());
		}
	}
	template<class T>
	LogProxy &operator<<(const T& val) {
		if (stream_) buffer_<< val;
		return *this;
	}
private:
	std::ostream *stream_;
	std::stringstream buffer_;
};


enum class EXPORT log_level {
	fatal 	= 0,
	info	= 1,
	debug	= 2
};
class EXPORT Log {
private:
	static log_level mode;
	std::ostream &stream;
public:
	Log(std::ostream& stream):stream(stream) {}
	void set_global_mode(log_level mode_) {
		mode = mode_;
	}
	LogProxy operator[](log_level level_) {
		if (!active(level_)) return LogProxy(nullptr);
		else return LogProxy(&stream);
	}
	inline bool active(log_level level) {
		return level<=mode;
	}
};
extern  EXPORT Log logger;

//#ifndef MODERN_COMPILER

/*!
 * @brief Ancillary class for initializing std::map
 *
 * Usage std::map<A, B> name_of_map = InitMap<A, B>(a0, b0)(a1, b1)(a2, b2);
 */
template<class Key, class Value> class InitMap {
public:
	operator std::map<Key, Value>() {
		return tmp_map;
	}
	InitMap() {}
	InitMap(Key key, Value value) {
		tmp_map.insert(std::make_pair(std::move(key), std::move(value)));
	}
	InitMap & operator() (Key key, Value value) {
		tmp_map.insert(std::make_pair(std::move(key), std::move(value)));
		return *this;
	}
protected:
	std::map<Key, Value> tmp_map;
};


//#endif

template<typename Out, typename In>
Out simple_cast(In&& src)
{
	std::stringstream sstr;
	Out tmp;
	sstr << src;
	sstr >> tmp;
	return tmp;
}

}



#endif /* UTILS_H_ */
