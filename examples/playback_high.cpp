/*
 * playback_high.cpp
 *
 *  Created on: 11.3.2013
 *      Author: neneko
 */


#include "iimavlib/AlsaSink.h"
#include "iimavlib/WaveSource.h"
#include "iimavlib/Utils.h"
#include <string>


int main(int argc, char** argv)
{
	using namespace iimavlib;
	/* ******************************************************************
	 *                      Process parameters
	 ****************************************************************** */
	if (argc<2) {
		logger[log_level::fatal] << "Not enough parameters. Specify the wave file please";
		return 1;
	}
	const std::string filename (argv[1]);
	logger[log_level::debug] << "Loading file " << filename;

	/* ******************************************************************
	 *                      Create and run the filter chain
	 ****************************************************************** */

	pAudioSink chain = filter_chain<WaveSource>(filename)
						.add<AlsaSink>("sysdefault")
						.sink();
	chain->run();

}
