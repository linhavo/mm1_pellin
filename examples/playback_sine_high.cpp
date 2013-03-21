/**
 * @file 	playback_sine_high.cpp
 *
 * @date 	11.3.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 * Sample program generating sine and playing it out using high level API.
 */


#include "iimavlib.h"
#include "iimavlib/WaveSource.h"
#include "iimavlib/Utils.h"
#include "iimavlib/AudioFilter.h"
#include <string>
#include <cassert>
#include <cmath>

using namespace iimavlib;

namespace {
// Max value for int16_t
const double max_val = std::numeric_limits<int16_t>::max();
// Value of 2*PI
const double pi2 = 8.0*std::atan(1.0);
}

class SineGenerator: public AudioFilter
{
public:
	SineGenerator(double frequency):AudioFilter(pAudioFilter()),
	frequency_(frequency),time_(0.0)
{
}
private:
	error_type_t do_process(audio_buffer_t& buffer)
	{
		// Prepare few values to save typing (and enable some optimizations)
		const audio_params_t& params = buffer.params;
		const size_t num_channels = params.num_channels;
		const size_t rate_int =convert_rate_to_int(params.rate);

		// Currently only 16bit signed samples are supported
		if (buffer.params.format != sampling_format_t::format_16bit_signed) {
			return error_type_t::unsupported;
		}


		// Get pointer to the raw data in the buffer (as a int16_t*)
		int16_t * data = reinterpret_cast<int16_t*>(&buffer.data[0]);

		// Iterate over all usable samples in the buffer
		for (size_t sample=0;sample<buffer.valid_samples;++sample) {
			// And for each sample, iterate over all channels
			for (size_t channel=0;channel<num_channels;++channel){
				data[sample*num_channels+channel]=static_cast<int16_t>(max_val*std::sin(time_*frequency_*pi2));
			}
			time_=time_+1.0/rate_int;
		}
		return error_type_t::ok;
	}

	double frequency_;
	double time_;
};

int main(int argc, char** argv) try
{
	/* ******************************************************************
	 *                      Process parameters
	 ****************************************************************** */
	if (argc<2) {
		logger[log_level::fatal] << "Not enough parameters. Specify the frequency, please.";
		logger[log_level::fatal] << "Usage: " << argv[0] << " frequency [audio_device]";
		return 1;
	}
	//const double frequency = std::stod(argv[1]);
	const double frequency = simple_cast<double>(argv[1]);
	logger[log_level::debug] << "Generating sine with frequency " << frequency << "Hz.";

	audio_id_t device_id = PlatformDevice::default_device();
	if (argc>2) {
		device_id = simple_cast<audio_id_t>(argv[2]);
	}
	logger[log_level::debug] << "Using audio device " << device_id;

	/* ******************************************************************
	 *                      Create and run the filter chain
	 ****************************************************************** */

	// Create filter chain
	auto chain = filter_chain<SineGenerator>(frequency)
						.add<PlatformSink>(device_id)
						.sink();

	// Start the filters
	chain->run();

	/*
	 * Alternative syntax would be:
	 * auto sine = std::make_shared<SineGenerator>(frequency);
	 * auto sink = std::make_shared<PlatformSink>(sine, device_id);
	 * sink->run();
	 *
	 */


}
catch (std::exception& e)
{
	logger[log_level::fatal] << "ERROR: An error occured during program run: " << e.what();
}
