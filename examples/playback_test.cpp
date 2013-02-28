/**
 * @file 	playback_test.cpp
 *
 * @date 	19.1.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 * Example playing an wave file
 */


#include "iimavlib.h"
#include "iimavlib/WaveFile.h"
#include "iimavlib/Utils.h"
#include <vector>

using namespace iimavlib;
int main()
{
	WaveFile wav("capture.wav");
	audio_params_t params = wav.get_params();
	logger[log_level::info] << "Opened wav file with " << static_cast<int>(params.num_channels) << " channels, "
			<< "sampling rate " << sampling_rate_string(params.rate) << " and "
			<< "sampling format '" << sampling_format_string(params.format) << "'";

	//PlaybackDevice device(params);
	MultithreadedPlaybackDevice device(params);
	const size_t buffer_size = 512;
	const size_t buffer_count = 4;
	device.set_buffers(buffer_count,buffer_size);
	std::vector<uint8_t> data(buffer_size*params.sample_size(),0);
	size_t frames;
	for (size_t i=0;i<buffer_count;++i) {
		frames = buffer_size;
		wav.read_data(data,frames);
		device.fill_buffer(data);
	}
	wav.read_data(data,frames);
	//bool running = true;
	device.start_playback();
	while (true) {
		error_type_t ret = device.update();
		if (ret == error_type_t::busy) continue;
		if (ret != error_type_t::ok) {
			logger[log_level::fatal] << "Failed to update";
			break;
		}
		ret = device.fill_buffer(data);
		if (ret == error_type_t::buffer_full) continue;
		if (ret != error_type_t::ok) break;
		logger[log_level::debug] << "Filled " << frames << " frames";
		frames = buffer_size;
		wav.read_data(data,frames);
		if (!frames) break;
	}




}
