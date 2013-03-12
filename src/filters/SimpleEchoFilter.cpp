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
/**
 * Adds 'echo' to the @em dest buffer, by adding weighted values from @em src.
 * @tparam T type of the samples. Should be signed.
 * @param dest Destination buffer to add echo to.
 * @param src Source buffer to read old values for echo.
 * @param count Number of samples to read.
 * @param decay Weighting of old and new values.
 */
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
	// Verify that we've got our supported format
	if (buffer.params.format != sampling_format_t::format_16bit_signed) {
		return error_type_t::unsupported;
	}
	if (buffer.valid_samples==0) return error_type_t::failed;

	// Some constant values that is convenient to have prepared
	const size_t frequency = convert_rate_to_int(buffer.params.rate);
	const size_t delay_samples = static_cast<size_t>(frequency*delay_);
	const size_t channels = buffer.params.num_channels;

	// Make sure old_samples_ is large enough
	old_samples_.resize(delay_samples*channels,0);

	// Pointer to the buffer after type conversion to int16_t
	int16_t * data = reinterpret_cast<int16_t*>(&buffer.data[0]);

	// Calculate how many samples from old_samples we're gonna use
	const size_t from_old = std::min(buffer.valid_samples, delay_samples);

	// And add echo to them (from old_samples_)
	add_echo(data, &old_samples_[0],from_old*channels, decay_);

	// if buffer.valid_samples is lesser or equal than delay_samples, we already have processed all the samples
	if (buffer.valid_samples <= delay_samples) {
		// Move unused part of the buffer to the beginning
		std::copy(old_samples_.begin()+from_old*channels,old_samples_.end(),old_samples_.begin());
		// Resize old_samples_ to the size of valid data in it
		old_samples_.resize(std::distance(old_samples_.begin()+from_old*channels,old_samples_.end()));
		// And insert samples from processed buffer
		old_samples_.insert(old_samples_.end(), data,data+buffer.valid_samples*channels);
	} else {
		// Our input buffer was larger then old_samples_, that means we still need to add echo to some samples
		//logger[log_level::debug] << "Too large input buffer or too small delay, not tested";
		// Add echo to the rest of input buffer
		add_echo(data+channels*from_old,data,(buffer.valid_samples-from_old)*channels,decay_);
		// We have no valid old samples
		old_samples_.resize(0);
		// Copy samples to old_samples_ from input buffer
		old_samples_.insert(old_samples_.end(),
						data+(buffer.valid_samples-delay_samples)*channels,
						data+buffer.valid_samples*channels);
	}
	return error_type_t::ok;
}
}






