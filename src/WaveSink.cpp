/*
 * WaveSink.cpp
 *
 *  Created on: 11.3.2013
 *      Author: neneko
 */

#include "iimavlib/WaveSink.h"
#include "iimavlib/Utils.h"
namespace iimavlib {

WaveSink::WaveSink(const pAudioFilter& child, const std::string& filename):
		AudioSink(child),file_(filename,get_params())
{
	const audio_params_t& params = file_.get_params();
	logger[log_level::info] << "Opened wav file with " << static_cast<int>(params.num_channels) << " channels, "
				<< "sampling rate " << sampling_rate_string(params.rate) << " and "
				<< "sampling format '" << sampling_format_string(params.format) << "'";
}
WaveSink::WaveSink(const pAudioFilter& child, const std::string& filename, const audio_params_t& params):
		AudioSink(child),file_(filename,params)

{

}
WaveSink::~WaveSink()
{
}
error_type_t WaveSink::do_run()
{
	const size_t buffer_size=512;
	audio_buffer_t buffer;
	buffer.params = file_.get_params();
	buffer.data.resize(buffer.params.sample_size()*buffer_size);
	std::fill(buffer.data.begin(),buffer.data.end(),0);
	while (still_running()) {
		buffer.valid_samples = buffer_size;
		if (process(buffer)!=error_type_t::ok) {
			stop();
			break;
		}
		//file_.store_data(buffer.data,buffer.valid_samples*buffer.params.sample_size()/buffer.params.num_channels);
	}
	return error_type_t::ok;
}

error_type_t WaveSink::do_process(audio_buffer_t& buffer)
{
	file_.store_data(buffer.data,buffer.valid_samples*buffer.params.sample_size()/buffer.params.num_channels);
	return error_type_t::ok;
}

}


