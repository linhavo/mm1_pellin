/*
 * SineMultiply.cpp
 *
 *  Created on: 11.3.2013
 *      Author: neneko
 */

#include "iimavlib/filters/SineMultiply.h"
#include <cmath>
#include <cassert>
namespace iimavlib {

SineMultiply::SineMultiply(const pAudioFilter& child, double frequency)
:AudioFilter(child),frequency_(frequency),time_(0.0)
{

}
SineMultiply::~SineMultiply()
{

}
namespace {
const double pi2 = 8*std::atan(1.0);
}
error_type_t SineMultiply::do_process(audio_buffer_t& buffer)
{
	const audio_params_t& params = buffer.params;
	assert(params.format == sampling_format_t::format_16bit_signed);
	int16_t * data = reinterpret_cast<int16_t*>(&buffer.data[0]);
	const size_t num_channels = params.num_channels;
	for (size_t sample=0;sample<buffer.valid_samples;++sample) {
		for (size_t channel=0;channel<num_channels;++channel){
			data[sample*num_channels+channel]*=static_cast<int16_t>(std::sin(time_*frequency_*pi2));
		}
		time_=time_+1.0/convert_rate_to_int(params.rate);
	}
	return error_type_t::ok;
}
}


