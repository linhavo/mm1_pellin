/*!
 * @file 		video_types.h
 * @author 		Zdenek Travnicek <travnicek@iim.cz>
 * @date 		25.2.2014
 * @copyright	Institute of Intermedia, CTU in Prague, 2013
 * 				Distributed under BSD Licence, details in file doc/LICENSE
 *
 */
#include "PlatformDefs.h"
#include <algorithm>
#include <ostream>
#include <type_traits>
#include <vector>
#include <cstdint>
#ifndef SYSTEM_WINDOWS
#include <initializer_list>
#endif

#ifndef VIDEO_TYPES_H_
#define VIDEO_TYPES_H_
namespace iimavlib {
struct EXPORT rectangle_t {
	int x;
	int y;
	int width;
	int height;

	rectangle_t(int x, int y, int width = 0, int height = 0):
		x(x),y(y),width(width),height(height) {}
	~rectangle_t() /*noexcept */{}
};

inline rectangle_t intersection(rectangle_t rect1, const rectangle_t& rect2)
{
	const int x = std::max(rect1.x, rect2.x);
	const int y = std::max(rect1.y, rect2.y);
	rect1.width = std::min (rect1.x + rect1.width, rect2.x + rect2.width) - x;
	rect1.height= std::min (rect1.y + rect1.height, rect2.y + rect2.height) - y;
	rect1.x 	= x;
	rect1.y 	= y;
	return rect1;
}
template<typename Type1, typename Type2>
inline Type1 mult_and_clamp(Type1 a, Type2 b)
{
	auto x = a * b;
	if (x > std::numeric_limits<Type1>::max()) {
		return std::numeric_limits<Type1>::max();
	}
	if (x < std::numeric_limits<Type1>::min()) {
		return std::numeric_limits<Type1>::min();
	}
	return static_cast<Type1>(x);
}
struct EXPORT rgb_t {
	uint8_t r:8;
	uint8_t g:8;
	uint8_t b:8;

	rgb_t(uint8_t r=0, uint8_t g = 0, uint8_t b = 0):
		r(r),g(g),b(b){}

	template<typename T>
	typename std::enable_if<std::is_arithmetic<T>::value,rgb_t&>::type
	operator*=(T val) { 
		r = mult_and_clamp(r, val);
		g = mult_and_clamp(g, val);
		b = mult_and_clamp(b, val);
		return *this; }
	template<typename T>
	typename std::enable_if<std::is_arithmetic<T>::value,rgb_t&>::type
	operator/=(T val) { r/=val; g/=val; b/=val; return *this; }
};

template<typename T>
typename std::enable_if<std::is_arithmetic<T>::value,rgb_t>::type
operator*(rgb_t rgb, T val) {
	rgb*=val;
	return rgb;
}
template<typename T>
typename std::enable_if<std::is_arithmetic<T>::value,rgb_t>::type
operator/(rgb_t rgb, T val) {
	rgb/=val;
	return rgb;
}
struct EXPORT video_buffer_t {

	typedef std::vector<rgb_t> data_type;
	typedef data_type::iterator iterator;
	typedef data_type::const_iterator const_iterator;
	rectangle_t size;
	std::vector<rgb_t> data;

	video_buffer_t():size(0,0,0,0) {}

	video_buffer_t(rectangle_t size, rgb_t color = rgb_t()):size(size){
		resize(size, color);
	}
	video_buffer_t(int width, int height, rgb_t color = rgb_t()):size(0,0,width,height){
		resize(size, color);
	}
	~video_buffer_t() /* noexcept */ {}

	void resize(int width, int height, rgb_t color= rgb_t()) {
		size = rectangle_t(0, 0, width, height);
		data.resize(width * height, color);
	}

	void resize(rectangle_t rect, rgb_t color = rgb_t()) {
		resize(rect.width, rect.height, color);
	}

	void clear(rgb_t color) { std::fill(begin(), end(), color);}


	rgb_t& operator()(int x, int y) { return data[x + y * size.width]; }
	rgb_t operator()(int x, int y) const { return data[x + y * size.width]; }

	iterator begin() { return data.begin(); }
	const_iterator begin() const { return data.begin(); }
	iterator end() { return data.end(); }
	const_iterator end() const { return data.end(); }

//	auto begin() { return data.begin(); }
};

inline std::ostream& operator<<(std::ostream& os, const rectangle_t& r)
{
	os << r.width << "x" << r.height << "+" << r.x << "+" << r.y;
	return os;
}

}

#endif /* VIDEO_TYPES_H_ */
