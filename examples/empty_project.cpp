/*!
 * @file 		empty_project.cpp
 * @author 		Zdenek Travnicek <travnicek@iim.cz>
 * @date 		6.3.2014
 * @copyright	Institute of Intermedia, CTU in Prague, 2013
 * 				Distributed under BSD Licence, details in file doc/LICENSE
 *
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
						.add<DefaultSink>(device_id)
						.sink();
	chain->run();

}
catch (std::exception& e)
{
	using namespace iimavlib;
	logger[log_level::fatal] << "ERROR: An error occured during program run: " << e.what();
}



