/**
 * @file 	FFT.h
 *
 * @date 	29.4.2016
 * @author 	Roman Berka <berka@fel.cvut.cz>
 * @copyright GNU Public License 3.0
 *
 */

#ifndef INCLUDE_IIMAVLIB_FFT_H_
#define INCLUDE_IIMAVLIB_FFT_H_

#include "iimavlib/ArrayTypes.h"

namespace iimavlib {
const int defaultWindowWidth = 64;

template<class IN, class OUT>
simplearray_t<OUT> powerSpectrum(const complexarray_t<IN> & coefficient_array) {
	simplearray_t<OUT> spectrum;
	for (const auto& coefficient: coefficient_array) {
		spectrum.push_back(static_cast<OUT>(std::abs(coefficient)));
	}

	return spectrum;
}

template <class T>
class FFT {
public:
	FFT():
		window{defaultWindowWidth},pi(4 * atan(1.0f)), M(8, 8)
	{
		for (auto k = 0;k < 8; ++k)
			for(auto n = 0; n < 8; ++n)
				M[k * 8 + n] = std::exp(pi * k * n / 8 * std::complex<T>(0, -2));
	}

	complexarray_t<T> DFT1D(const simplearray_t<T> &ab);
	complexarray_t<T> FFT1D(const simplearray_t<T> &ab);

	void setWidth (int w)
	{
		window = w;
	}

	T hann(int k, int N);
private:
	int window;
	T pi;
	// Precalculated coefficients for DFT (8x8)
	matrix<std::complex<T> > M;
};


template <class T>
complexarray_t<T> FFT<T>::DFT1D(const simplearray_t<T> &ab){
	if (ab.size() != 8)
		throw std::runtime_error("DFT should be called for exactly 8 samples");

	auto zz = (M * ab).data();
	return zz;
}

template <class T>
T FFT<T>::hann(int k, int N){
	T x = 0.5 * (1.0 - std::cos(2 * pi * k / N));
	return x;
}

template <class T>
complexarray_t<T> FFT<T>::FFT1D(const simplearray_t<T> &ab) {
	const auto N = ab.size();

	if(N == 0 || (N & (N - 1))) {
		throw std::runtime_error("FFT Error: the input number of samples must be a power of 2!");
	} else if (N <= 8) {
		return DFT1D(ab);
	} else {
		simplearray_t<T> samples_odd;
		samples_odd.reserve(N / 2);
		for(auto i = 0u; i < N; i += 2)
			samples_odd.push_back(ab[i]);
		auto coefficients_odd = FFT1D(samples_odd);

		simplearray_t<T> samples_even;
		samples_even.reserve(N / 2);
		for(auto i = 1u; i < N; i += 2)
			samples_even.push_back(ab[i]);
		auto coefficients_even = FFT1D(samples_even);

		complexarray_t<T> f;
		for(auto n = 0u; n < N; n++) {
			f.push_back(std::exp(pi * n / N * std::complex<T>(0, -2)));
		}

		complexarray_t<T> result(N, 0);
		for(auto i = 0u; i < N / 2; ++i) {
			result[i] = coefficients_odd[i] + f[i] * coefficients_even[i];
			result[i + N / 2] = coefficients_odd[i] + f[i + N / 2] * coefficients_even[i];
		}
		return result;
	}
}

}

#endif /* INCLUDE_IIMAVLIB_FFT_H_ */
