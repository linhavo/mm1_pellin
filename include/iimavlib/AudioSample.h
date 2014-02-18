/*!
 * @file 		AudioSample.h
 * @author 		Zdenek Travnicek <travnicek@iim.cz>
 * @date 		18.2.2014
 * @copyright	Institute of Intermedia, CTU in Prague, 2013
 * 				Distributed under BSD Licence, details in file doc/LICENSE
 *
 */


#ifndef AUDIOSAMPLE_H_
#define AUDIOSAMPLE_H_

#include <stdexcept>
#include <type_traits>

struct audio_sample_t {
	audio_sample_t():left(0),right(0) {}
	audio_sample_t(int16_t val):left(val),right(val) {}
	audio_sample_t(int16_t vall, int16_t valr):left(vall),right(valr) {}

	int16_t left;
	int16_t right;

	int16_t& operator[](size_t i) {
		if (i==0) {
			return left;
		}
		if (i==1) {
			return right;
		}
		throw std::out_of_range("BUM");
	}

	int16_t operator[](size_t i) const {
		if (i==0) {
			return left;
		}
		if (i==1) {
			return right;
		}
		throw std::out_of_range("BUM");
	}

	audio_sample_t& operator+=(const audio_sample_t& rhs)
	{
		left  += rhs.left;
		right += rhs.right;
		return *this;
	}

	template<typename T>
	typename std::enable_if<std::is_arithmetic<T>::value,audio_sample_t&>::type
	operator*=(T value) {
		left  *= value;
		right *= value;
		return *this;
	}
	template<typename T>
	typename std::enable_if<std::is_arithmetic<T>::value,audio_sample_t&>::type
	operator/=(T value) {
		left  /= value;
		right /= value;
		return *this;
	}
};


inline audio_sample_t operator+(audio_sample_t lhs, const audio_sample_t& rhs)
{
	lhs += rhs;
	return lhs;
}


template<typename T>
typename std::enable_if<std::is_arithmetic<T>::value,audio_sample_t>::type
operator*(audio_sample_t sample, T value)
{
	sample*=value;
	return sample;
}

template<typename T>
typename std::enable_if<std::is_arithmetic<T>::value,audio_sample_t>::type
operator*(T value, audio_sample_t sample)
{
	sample*=value;
	return sample;
}

template<typename T>
typename std::enable_if<std::is_arithmetic<T>::value,audio_sample_t>::type
operator/(audio_sample_t sample, T value)
{
	sample/=value;
	return sample;
}

template<typename T>
typename std::enable_if<std::is_arithmetic<T>::value,audio_sample_t>::type
operator/(T value, audio_sample_t sample)
{
	sample/=value;
	return sample;
}



#endif /* AUDIOSAMPLE_H_ */
