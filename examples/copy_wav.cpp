/*
 * copy_wav.cpp
 *
 *  Created on: 11.3.2013
 *      Author: neneko
 */


#include "iimavlib/WaveSink.h"
#include "iimavlib/WaveSource.h"
#include "iimavlib/Utils.h"
#include <string>


int main(int argc, char** argv)
{
	using namespace iimavlib;
	/* ******************************************************************
	 *                Process command line parameters
	 ****************************************************************** */
	if (argc<3) {
		logger[log_level::fatal] << "Not enough parameters. Specify the in and out wave files.";
		return 1;
	}


	const std::string filename (argv[1]);
	const std::string filename2 (argv[2]);
	logger[log_level::debug] << "Copying " << filename << " -> " << filename2;

	/* ******************************************************************
	 *                Create and run the filter chain
	 ****************************************************************** */

	pAudioSink chain = filter_chain<WaveSource>(filename)
						.add<WaveSink>(filename2)
						.sink();
	chain->run();

}


