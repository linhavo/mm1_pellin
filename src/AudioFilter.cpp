/**
 * @file 	AudioFilter.cpp
 *
 * @date 	27.2.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
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
pAudioFilter AudioFilter::get_child(size_t depth)
{
	if (!depth) return child_;
	if (!child_) return pAudioFilter();
	return child_->get_child(depth-1);
}
audio_params_t AudioFilter::do_get_params() const
{
	if (child_) {
		return child_->get_params();
	}
	return audio_params_t();
}

}
