/*
 * AlsaSink.cpp
 *
 *  Created on: 11.3.2013
 *      Author: neneko
 */

#include "iimavlib/AlsaSink.h"
#include "iimavlib/Utils.h"

namespace iimavlib {
AlsaSink::AlsaSink(const pAudioFilter& child_, const audio_params_t& params, AlsaDevice::audio_id_t id):
		AudioSink(child_),device_(action_type_t::action_playback, id, params),
		params_(params),
		buffer_count_(4),buffer_size_(512)
{
	init_buffers();
}

AlsaSink::AlsaSink(const pAudioFilter& child_, AlsaDevice::audio_id_t id):
		AudioSink(child_),device_(action_type_t::action_playback, id, get_params()),
		params_(device_.do_get_params()),
		buffer_count_(4),buffer_size_(512)
{
	init_buffers();
}
void AlsaSink::init_buffers()
{
	device_.do_set_buffers(buffer_count_, buffer_size_);
	buffer_.data.resize(buffer_size_*params_.sample_size(),0);
	buffer_.params = params_;
}
AlsaSink::~AlsaSink()
{

}

error_type_t AlsaSink::do_run()
{

	for (size_t i=0;i<buffer_count_;++i) {
		process(buffer_);
		device_.do_fill_buffer(&buffer_.data[0],buffer_.valid_samples*params_.sample_size());
	}
	device_.do_start_playback();
	while (still_running()) {
		error_type_t ret = device_.do_update();
		if (ret == error_type_t::busy) continue;
		if (ret != error_type_t::ok) {
			logger[log_level::fatal] << "Failed to update";
			stop();
			break;
		}
		buffer_.valid_samples = buffer_size_;
		ret = device_.do_fill_buffer(&buffer_.data[0],buffer_.valid_samples*params_.sample_size());
		if (ret == error_type_t::buffer_full) continue;
		if (ret != error_type_t::ok) {
			stop();
			break;
		}
//		logger[log_level::debug] << "Filled " << buffer_.valid_samples << " samples";

		process(buffer_);

	}
	return error_type_t::ok;
}



}


