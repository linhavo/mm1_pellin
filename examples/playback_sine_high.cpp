/*
 * playback_sine_high.cpp
 *
 *  Created on: 11.3.2013
 *      Author: neneko
 */


/*
 * playback_high.cpp
 *
 *  Created on: 11.3.2013
 *      Author: neneko
 */


#include "iimavlib/AlsaSink.h"
#include "iimavlib/WaveSource.h"
#include "iimavlib/Utils.h"
#include "iimavlib/AudioFilter.h"
#include <string>
#include <cassert>
#include <cmath>

using namespace iimavlib;

namespace {
const double max_val = std::numeric_limits<int16_t>::max();
const double pi2 = 8.0*std::atan(1.0);
}
class SineGenerator: public AudioFilter
{
public:
	SineGenerator(double frequency):AudioFilter(pAudioFilter()),frequency_(frequency),time_(0)
{
}
private:
	error_type_t do_process(audio_buffer_t& buffer)
	{
		const audio_params_t& params = buffer.params;
		assert(params.format == sampling_format_t::format_16bit_signed);
		int16_t * data = reinterpret_cast<int16_t*>(&buffer.data[0]);
		const size_t num_channels = params.num_channels;
		for (size_t sample=0;sample<buffer.valid_samples;++sample) {
			for (size_t channel=0;channel<num_channels;++channel){
				data[sample*num_channels+channel]=static_cast<int16_t>(max_val*std::sin(time_*frequency_*pi2));
			}
			time_=time_+1.0/convert_rate_to_int(params.rate);
		}
		return error_type_t::ok;
	}

	double frequency_;
	double time_;
};

int main(int argc, char** argv)
{
	/* ******************************************************************
	 *                      Process parameters
	 ****************************************************************** */
	if (argc<2) {
		logger[log_level::fatal] << "Not enough parameters. Specify the frequency, please.";
		return 1;
	}
	const double frequency = std::stod(argv[1]);
	logger[log_level::debug] << "Generating sine with frequency " << frequency << "Hz.";

	/* ******************************************************************
	 *                      Create and run the filter chain
	 ****************************************************************** */

	pAudioSink chain = filter_chain<SineGenerator>(frequency)
						.add<AlsaSink>("sysdefault")
						.sink();
	chain->run();

}



