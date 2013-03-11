/*
 * AudioFilter.cpp
 *
 *  Created on: 27.2.2013
 *      Author: neneko
 */

#include "iimavlib/AudioFilter.h"

namespace iimavlib {

AudioFilter::AudioFilter(const pAudioFilter& child):child_(child)
{

}
AudioFilter::~AudioFilter()
{

}

error_type_t AudioFilter::process(audio_buffer_t& buffer)
{
	error_type_t ret = error_type_t::ok;

	if (child_) {
		ret = child_->process(buffer);
		if (ret != error_type_t::ok) return ret;
	}

	return do_process(buffer);
}
audio_params_t AudioFilter::get_params() const
{
	return do_get_params();
}

audio_params_t AudioFilter::do_get_params() const
{
	if (child_) {
		return child_->get_params();
	}
	return audio_params_t();
}
}
