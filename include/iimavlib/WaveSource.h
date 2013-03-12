/*
 * WaveSource.h
 *
 *  Created on: 11.3.2013
 *      Author: neneko
 */

#ifndef WAVESOURCE_H_
#define WAVESOURCE_H_

#include "WaveFile.h"
#include "AudioFilter.h"
#include <string>

namespace iimavlib {
class EXPORT WaveSource: public AudioFilter {
public:
	WaveSource(const std::string filename);
	virtual ~WaveSource();

private:
	virtual error_type_t do_process(audio_buffer_t& buffer);
	virtual audio_params_t do_get_params() const;
	WaveFile file_;
};

}



#endif /* WAVESOURCE_H_ */
