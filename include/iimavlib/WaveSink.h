/*
 * WaveSink.h
 *
 *  Created on: 11.3.2013
 *      Author: neneko
 */

#ifndef WAVESINK_H_
#define WAVESINK_H_

#include "AudioSink.h"
#include "WaveFile.h"
#include <string>
namespace iimavlib {

class WaveSink: public AudioSink
{
public:
	WaveSink(const pAudioFilter& child, const std::string& filename);
	WaveSink(const pAudioFilter& child, const std::string& filename, const audio_params_t& params);
	virtual ~WaveSink();
private:
	virtual error_type_t do_run();
	WaveFile file_;
};

}
#endif /* WAVESINK_H_ */
