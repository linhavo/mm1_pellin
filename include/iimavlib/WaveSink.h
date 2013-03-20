/**
 * @file 	WaveSink.h
 *
 * @date 	11.3.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 * This file defines sink filter for wave files
 */

#ifndef WAVESINK_H_
#define WAVESINK_H_

#include "AudioSink.h"
#include "WaveFile.h"
#include <string>
namespace iimavlib {

class EXPORT WaveSink: public AudioSink
{
public:
	WaveSink(const pAudioFilter& child, const std::string& filename);
	WaveSink(const pAudioFilter& child, const std::string& filename, const audio_params_t& params);
	virtual ~WaveSink();
private:
	virtual error_type_t do_run();
	virtual error_type_t do_process(audio_buffer_t& buffer);
	WaveFile file_;
};

}
#endif /* WAVESINK_H_ */
