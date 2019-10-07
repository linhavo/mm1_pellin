/*
 * sdl_spectrum.cpp
 *
 *  Created on: 18. 3. 2016
 *      Author: berka
 */


#include "iimavlib/AudioFFT.h"
#include "iimavlib/AudioTypes.h"

//#include <chrono>

int main() {
	using namespace iimavlib;
	std::cout << "ENTER: ";
	std::cout << std::fixed;
	std::cout.precision(3);
//	simplearray_t<float> s = {0.4, 0.5, 0.6, 0.9, 0.89, 0.1, 0.3, 0.99, 0.4, 0.5, 0.6, 0.9, 0.89, 0.1, 0.3, 0.99};
//	simplearray_t<float> s = {0.4, 0.5, 0.6, 0.9, 0.89, 0.1, 0.3, 0.91, 0.44, 0.4, 0.32, 0.56, 0.23, 0.13, 0.1};
	simplearray_t<float> s;
	float t = 0.0, omega=2.0*3.14159265*1.0;
	for (auto i = 0 ;i < 64;i++,t+=1.0/64.0)
		s.push_back(10000*sin(t*omega));

	std::cout << s << std::endl;
	complexarray_t<float>  zz;
	FFT<float> dsp;
//	auto t0 = std::chrono::high_resolution_clock::now();
	zz = dsp.FFT1D(s);
	simplearray_t<int> out = powerSpectrum<float,int>(zz);
//	auto t1 = std::chrono::high_resolution_clock::now();
	std::cout << out;
//	std::cout << "FFT took " << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() << " ms\n";

}
