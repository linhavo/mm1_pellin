/**
 * @file 	SineMultiply.h
 *
 * @date 	11.3.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 * This file declares filter multiplying the signal with a sine
 */

#ifndef SINEMULTIPLY_H_
#define SINEMULTIPLY_H_

#include "../AudioFilter.h"
namespace iimavlib {
class EXPORT SineMultiply: public AudioFilter {
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
