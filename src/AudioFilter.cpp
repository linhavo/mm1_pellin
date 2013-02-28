/*
 * AudioFilter.cpp
 *
 *  Created on: 27.2.2013
 *      Author: neneko
 */

#include "iimavlib/AudioFilter.h"

namespace iimavlib {

AudioFilter::AudioFilter(pAudioFilter parent):parent_(parent)
{

}
AudioFilter::~AudioFilter()
{

}

error_type_t AudioFilter::process(audio_buffer_t& buffer)
{
	error_type_t ret = error_type_t::ok;
	if (parent_) {
		ret = parent_->process(buffer);
		if (ret != error_type_t::ok) return ret;
	}
	return do_process(buffer);
}
}
