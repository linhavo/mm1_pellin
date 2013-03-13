/*
 * WinMMSink.cpp
 *
 *  Created on: 11.3.2013
 *      Author: neneko
 */

#include "iimavlib/WinMMSink.h"
#include "iimavlib/Utils.h"

namespace iimavlib {
WinMMSink::WinMMSink(const pAudioFilter& child_, const audio_params_t& params, WinMMDevice::audio_id_t id):
		AudioSink(child_),device_(action_type_t::action_playback, id, params),
		params_(params),
		buffer_count_(8),buffer_size_(1024)
{
	init_buffers();
}

WinMMSink::WinMMSink(const pAudioFilter& child_, WinMMDevice::audio_id_t id):
		AudioSink(child_),device_(action_type_t::action_playback, id, get_params()),
		params_(device_.do_get_params()),
		buffer_count_(8),buffer_size_(1024)
{
	init_buffers();
}
void WinMMSink::init_buffers()
{
	device_.do_set_buffers(static_cast<uint16_t>(buffer_count_), static_cast<uint32_t>(buffer_size_));
	buffer_.data.resize(buffer_size_*params_.sample_size(),0);
	buffer_.params = params_;
}
WinMMSink::~WinMMSink()
{

}

error_type_t WinMMSink::do_run()
{

	for (size_t i=0;i<buffer_count_;) {
		if (!still_running()) break;
		buffer_.valid_samples = buffer_size_;
		if (process(buffer_)!=error_type_t::ok) {
			stop();
			break;
		}
		if (buffer_.valid_samples==0) continue;
		device_.do_fill_buffer(&buffer_.data[0],buffer_.valid_samples*params_.sample_size());
		++i;
	}
	device_.do_start_playback();
	while (still_running()) {
		error_type_t ret = device_.do_update();
		if (ret == error_type_t::busy) continue;
		if (ret != error_type_t::ok) {
			logger[log_level::fatal] << "Failed to update";
			break;
		}
		buffer_.valid_samples = buffer_size_;
		logger[log_level::debug] << "Storing " << buffer_.valid_samples*params_.sample_size() << " bytes";
		ret = device_.do_fill_buffer(&buffer_.data[0],buffer_.valid_samples*params_.sample_size());
		if (ret == error_type_t::buffer_full) continue;
		if (ret != error_type_t::ok) {
			break;
		}


		if (process(buffer_)!=error_type_t::ok) break;
		logger[log_level::debug] << "Filled " << buffer_.valid_samples << " samples";
	}
	stop(); // Not necessarily needed, but it seems cleaner.
	return error_type_t::ok;
}


void WinMMSink::do_set_buffers(size_t count, size_t size)
{
	buffer_count_ = count;
	buffer_size_ = size;
	init_buffers();
}

}


