/*
 * AudioFilter.h
 *
 *  Created on: 27.2.2013
 *      Author: neneko
 */

#ifndef AUDIOFILTER_H_
#define AUDIOFILTER_H_
#include "AudioTypes.h"
#include "PlatformDefs.h"
#include <memory>
#include <vector>

namespace iimavlib {

typedef EXPORT std::shared_ptr<class AudioFilter> pAudioFilter;

class EXPORT AudioFilter {
public:
	AudioFilter(const pAudioFilter& child);
	virtual ~AudioFilter();
	error_type_t process(audio_buffer_t& buffer);
	audio_params_t get_params() const;
private:
	virtual error_type_t do_process(audio_buffer_t& buffer) = 0;
	virtual audio_params_t do_get_params() const;
	pAudioFilter child_;
};


}


#endif /* AUDIOFILTER_H_ */
