/*
 * playthrough_high.cpp
 *
 *  Created on: 12.3.2013
 *      Author: neneko
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


	audio_params_t params(sampling_rate_t::rate_48kHz, sampling_format_t::format_16bit_signed, 1);
	auto filters = filter_chain<PlatformSource>(params,device_in)
						.add<NullFilter>()
						.add<SimpleEchoFilter>(0.2);
	if (!out_file.empty()) filters = filters
						.add<WaveSink>(out_file);
	auto chain = filters.add<PlatformSink>(device_out)
						.sink();

	chain->set_buffers(4,2048);
	chain->run();

}
catch (std::exception& e)
{
	using namespace iimavlib;
	logger[log_level::fatal] << "ERROR: An error occured during program run: " << e.what();
}
