/*
 * WaveSource.cpp
 *
 *  Created on: 11.3.2013
 *      Author: neneko
 */


#include "iimavlib/WaveSource.h"
#include "iimavlib/Utils.h"
namespace iimavlib {

WaveSource::WaveSource(const std::string filename):AudioFilter(pAudioFilter()),file_(filename)
{

}

WaveSource::~WaveSource()
{

}
error_type_t WaveSource::do_process(audio_buffer_t& buffer)
{
//	logger[log_level::debug] << "[WaveSource] Processing buffer";
	return file_.read_data(buffer.data,buffer.valid_samples);
}

audio_params_t WaveSource::do_get_params() const {
	return file_.get_params();
}

}


