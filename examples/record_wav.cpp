/*
 * record_wav.cpp
 *
 *  Created on: 6.3.2014
 *      Author: neneko
 */

#include "iimavlib.h"
#include "iimavlib/WaveSink.h"
#include "iimavlib/Utils.h"

int main(int argc, char** argv) try
{
	using namespace iimavlib;

	/* ******************************************************************
	 *                Process command line parameters
	 ****************************************************************** */

	audio_id_t device_in = PlatformDevice::default_device();

	std::string out_file;

	if (argc < 2) {
		logger[log_level::fatal] << "Usage: " << argv[0] << " filename.wav [audio_device]";
		return 1;
	}
	out_file = simple_cast<std::string>(argv[1]);
	if (argc>2) device_in = simple_cast<audio_id_t>(argv[2]);
	/* ******************************************************************
	 *                Create and run the filter chain
	 ****************************************************************** */


	audio_params_t params(sampling_rate_t::rate_48kHz);

	// Create filter chain audio capture -> null -> simple_echo
	auto chain = filter_chain<PlatformSource>(params,device_in)
						.add<WaveSink>(out_file)
						.sink();

	chain->run();

}
catch (std::exception& e)
{
	using namespace iimavlib;
	logger[log_level::fatal] << "ERROR: An error occured during program run: " << e.what();
}




