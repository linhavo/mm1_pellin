/**
 * @file 	playback_high.cpp
 *
 * @date 	11.3.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 * Simple example reading a wave file and playing it out using high level API.
 */


#include "iimavlib.h"
#include "iimavlib/WaveSource.h"
#include "iimavlib/filters/SimpleEchoFilter.h"
#include "iimavlib/Utils.h"
#include <string>


int main(int argc, char** argv) try
{
	using namespace iimavlib;
	/* ******************************************************************
	 *                Process command line parameters
	 ****************************************************************** */
	if (argc<2) {
		logger[log_level::fatal] << "Not enough parameters. Specify the wave file please";
		return 1;
	}
	const std::string filename (argv[1]);
	logger[log_level::debug] << "Loading file " << filename;

	audio_id_t device_id = PlatformDevice::default_device();
	if (argc>2) {
		device_id = simple_cast<audio_id_t>(argv[2]);
	}
	/* ******************************************************************
	 *                Create and run the filter chain
	 ****************************************************************** */

	auto chain = filter_chain<WaveSource>(filename)
						.add<SimpleEchoFilter>(0.2)
						.add<DefaultSink>(device_id)

						.sink();
	chain->run();

}
catch (std::exception& e)
{
	using namespace iimavlib;
	logger[log_level::fatal] << "ERROR: An error occured during program run: " << e.what();
}
