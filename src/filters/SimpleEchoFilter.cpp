/*
 * EchoFilter.cpp
 *
 *  Created on: 12.3.2013
 *      Author: neneko
 */

#include "iimavlib/filters/SimpleEchoFilter.h"
#include "iimavlib/Utils.h"
namespace iimavlib {

SimpleEchoFilter::SimpleEchoFilter(const pAudioFilter& child, double delay, double decay)
:AudioFilter(child),delay_(delay),decay_(decay)
{

}
SimpleEchoFilter::~SimpleEchoFilter()
{

}
namespace {
template<typename T>
void add_echo(T* dest, const T *src, size_t count, double decay)
{
	for (size_t sample = 0; sample < count; ++sample) {
		*dest = static_cast<T>(decay * *src++ + (1.0-decay)* *dest);
		dest++;
	}
}
}
error_type_t SimpleEchoFilter::do_process(audio_buffer_t& buffer)
{
	if (buffer.params.format != sampling_format_t::format_16bit_signed) {
		return error_type_t::unsupported;
	}
	if (buffer.valid_samples==0) return error_type_t::failed;
	const size_t frequency = convert_rate_to_int(buffer.params.rate);
	const size_t delay_samples = static_cast<size_t>(frequency*delay_);
	const size_t channels = buffer.params.num_channels;
//	logger[log_level::debug] << "Delaying by " << delay_samples << " samples";
	old_samples_.resize(delay_samples*channels,0);
	int16_t * data = reinterpret_cast<int16_t*>(&buffer.data[0]);
	const size_t from_old = std::min(buffer.valid_samples, delay_samples);
//	if (old_samples_.size() < from_old*channels)
//		old_samples_.resize(from_old*channels,0);
	add_echo(data, &old_samples_[0],from_old*channels, decay_);
	if (from_old < delay_samples) {
		std::copy(old_samples_.begin()+from_old*channels,old_samples_.end(),old_samples_.begin());
//		logger[log_level::debug] << "Copied " << std::distance(old_samples_.begin()+from_old*channels,old_samples_.end()) << " bytes";
		old_samples_.resize(std::distance(old_samples_.begin()+from_old*channels,old_samples_.end()));
		old_samples_.insert(old_samples_.end(),//begin()+(delay_samples - from_old)*channels,
				data,data+buffer.valid_samples*channels);
//		logger[log_level::debug] << "Inserted: " << std::distance(data,data+buffer.valid_samples*channels);
	} else {
		logger[log_level::debug] << "Too large input buffer or too small delay, not tested";
		add_echo(data+channels*from_old,data,(buffer.valid_samples-from_old)*channels,decay_);
		old_samples_.resize(0);
		old_samples_.insert(old_samples_.begin(),
						data+(buffer.valid_samples-delay_samples)*channels,
						data+buffer.valid_samples*channels);
	}
//	logger[log_level::debug] << "old samples size: " << old_samples_.size();
	return error_type_t::ok;
}
}






