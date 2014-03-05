/**
 * @file 	playthrough_high.cpp
 *
 * @date 	12.3.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 * Simple example for capturing audio, adding echo and playing it out using high level API.
 */

#include "iimavlib.h"
#include "iimavlib/WaveSink.h"
#include "iimavlib/filters/NullFilter.h"
#include "iimavlib/filters/SimpleEchoFilter.h"
#include "iimavlib/Utils.h"

int main(int argc, char** argv) try
{
	using namespace iimavlib;

	/* ******************************************************************
	 *                Process command line parameters
	 ****************************************************************** */

	audio_id_t device_in = PlatformDevice::default_device();
	audio_id_t device_out = PlatformDevice::default_device();
	std::string out_file;
	if (argc>1) device_in = simple_cast<audio_id_t>(argv[1]);
	if (argc>2) device_out = simple_cast<audio_id_t>(argv[2]);
	if (argc>3) out_file = simple_cast<std::string>(argv[3]);
	/* ******************************************************************
	 *                Create and run the filter chain
	 ****************************************************************** */


	audio_params_t params(sampling_rate_t::rate_48kHz);//, sampling_format_t::format_16bit_signed, 1);

	// Create filter chain audio capture -> null -> simple_echo
	auto filters = filter_chain<PlatformSource>(params,device_in)
						.add<NullFilter>()
						.add<SimpleEchoFilter>(0.2);

	// If there's an output file specified, let's add WaveSink filter to the chain
	if (!out_file.empty()) filters = filters
						.add<WaveSink>(out_file);

	// And finally add audio sink
	auto chain = filters.add<PlatformSink>(device_out)
						.sink();

	assert(chain);
	chain->run();

}
catch (std::exception& e)
{
	using namespace iimavlib;
	logger[log_level::fatal] << "ERROR: An error occured during program run: " << e.what();
}
