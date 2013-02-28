/*
 * AudioFilter.h
 *
 *  Created on: 27.2.2013
 *      Author: neneko
 */

#ifndef AUDIOFILTER_H_
#define AUDIOFILTER_H_
#include "AudioTypes.h"
#include <memory>

namespace iimavlib {

typedef std::shared_ptr<class AudioFilter> pAudioFilter;

class AudioFilter {
public:
	AudioFilter(pAudioFilter parent);
	virtual ~AudioFilter();
	error_type_t process(audio_buffer_t& buffer);
private:
	virtual error_type_t do_process(audio_buffer_t& buffer) = 0;
	pAudioFilter parent_;
};

}


#endif /* AUDIOFILTER_H_ */
