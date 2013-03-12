/*
 * Echo.h
 *
 *  Created on: 12.3.2013
 *      Author: neneko
 */

#ifndef ECHO_H_
#define ECHO_H_

#include "../AudioFilter.h"
namespace iimavlib {
class EXPORT SimpleEchoFilter: public AudioFilter {
public:
	SimpleEchoFilter(const pAudioFilter& child, double delay=0.3, double decay=0.5);
	virtual ~SimpleEchoFilter();
private:
	virtual error_type_t do_process(audio_buffer_t& buffer);
	std::vector<int16_t> old_samples_;
	double delay_;
	double decay_;
};

}


#endif /* ECHO_H_ */
