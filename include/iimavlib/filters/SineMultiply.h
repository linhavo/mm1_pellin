/*
 * SineMultiply.h
 *
 *  Created on: 11.3.2013
 *      Author: neneko
 */

#ifndef SINEMULTIPLY_H_
#define SINEMULTIPLY_H_

#include "../AudioFilter.h"
namespace iimavlib {
class SineMultiply: public AudioFilter {
public:
	SineMultiply(const pAudioFilter& child, double frequency);
	virtual ~SineMultiply();
private:
	virtual error_type_t do_process(audio_buffer_t& buffer);
	double frequency_;
	double time_;
};

}
#endif /* SINEMULTIPLY_H_ */
