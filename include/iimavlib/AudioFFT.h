/**
 * @file 	AudioFFT.h
 *
 * @date 	29.4.2016
 * @author 	Roman Berka <berka@fel.cvut.cz>
 * @copyright GNU Public License 3.0
 *
 */

#ifndef INCLUDE_IIMAVLIB_AUDIOFFT_H_
#define INCLUDE_IIMAVLIB_AUDIOFFT_H_

#include "iimavlib/AudioTypes.h"
#include "iimavlib/FFT.h"
#include <algorithm>

namespace iimavlib {

template<class T>
class AudioFFT: FFT<T> {
public:
	AudioFFT():FFT<T>::FFT(){}
	complexarray_t<T> FFT1D(const std::vector<audio_sample_t>::iterator s, const std::vector<audio_sample_t>::iterator e);
};

template<class T>
complexarray_t<T> AudioFFT<T>::FFT1D(const std::vector<audio_sample_t>::iterator s, const std::vector<audio_sample_t>::iterator e) {
	simplearray_t<T> bf;
	bf.resize(std::distance(s, e));

	std::transform(s, e, bf.begin(), [](const audio_sample_t& sample){return sample.left;});

	return FFT<T>::FFT1D(bf);
}

}
#endif /* INCLUDE_IIMAVLIB_AUDIOFFT_H_ */
