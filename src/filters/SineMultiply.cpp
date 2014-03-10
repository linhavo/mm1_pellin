/**
 * @file 	SineMultiply.cpp
 *
 * @date 	11.3.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
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
	const double step = 1.0/convert_rate_to_int(params.rate);
	for (auto& sample: buffer.data) {
		sample = sample * std::sin(time_*frequency_*pi2);
		time_=time_+ step;
	}
	return error_type_t::ok;
}
}


