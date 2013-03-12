/*
 * WinMMSource.cpp
 *
 *  Created on: 12.3.2013
 *      Author: neneko
 */

#include "iimavlib/WinMMSource.h"
#include "iimavlib/Utils.h"

namespace iimavlib {

WinMMSource::WinMMSource(const audio_params_t& params, WinMMDevice::audio_id_t id):
AudioFilter(pAudioFilter()),device_(action_type_t::action_capture, id, params)
{
	if (device_.do_start_capture()!=error_type_t::ok) {
		logger[log_level::fatal] << "Failed to start capture";
	}
}
WinMMSource::~WinMMSource()
{

}

error_type_t WinMMSource::do_process(audio_buffer_t& buffer)
{
	const audio_params_t& params = buffer.params;
	error_type_t err;
	size_t captured = device_.do_capture_data(&buffer.data[0],buffer.valid_samples*params.sample_size(),err);
	if (err == error_type_t::xrun) {
		logger[log_level::info] << "An overrun occured!";
	} else if (err !=error_type_t::ok) {
		logger[log_level::fatal] << "An error occured: " << error_string(err);
		return error_type_t::ok;
	}
	buffer.valid_samples = captured;
//	logger[log_level::debug] << "Captured " << captured << " samples";
	return error_type_t::ok;

}
audio_params_t WinMMSource::do_get_params() const
{
	return device_.do_get_params();
}

}
