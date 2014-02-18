/**
 * @file 	SimpleEchoFilter.h
 *
 * @date 	12.3.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 * This file declares filter adding simple echo to the samples
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
	std::vector<audio_sample_t> old_samples_;
	double delay_;
	double decay_;
};

}


#endif /* ECHO_H_ */
