/**
 * @file 	WaveSource.cpp
 *
 * @date 	11.3.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
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
	error_type_t ret = file_.read_data(buffer.data,buffer.valid_samples);
	if (buffer.valid_samples==0) return error_type_t::failed;
	return ret;
}

audio_params_t WaveSource::do_get_params() const {
	return file_.get_params();
}

}


