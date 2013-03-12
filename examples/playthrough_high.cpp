/*
 * playthrough_high.cpp
 *
 *  Created on: 12.3.2013
 *      Author: neneko
 */

#include "iimavlib/AlsaSource.h"
#include "iimavlib/AlsaSink.h"
#include "iimavlib/WaveSink.h"
#include "iimavlib/filters/NullFilter.h"
#include "iimavlib/filters/SimpleEchoFilter.h"

int main(int argc, char** argv)
{
	using namespace iimavlib;
	audio_params_t params(sampling_rate_t::rate_48kHz, sampling_format_t::format_16bit_signed, 1);
	pAudioSink chain = filter_chain<AlsaSource>(params,"hw:2,0")
						.add<NullFilter>()
						.add<SimpleEchoFilter>(0.1)
						.add<AlsaSink>("sysdefault")
//						.add<WaveSink>("out.wav")
						.sink();
	chain->set_buffers(4,2048);
	chain->run();

}

