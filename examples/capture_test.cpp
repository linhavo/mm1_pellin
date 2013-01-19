/*
 * capture_test.cpp
 *
 *  Created on: 19.1.2013
 *      Author: neneko
 */

#include "iimaudio.h"
#include "iimaudio/Utils.h"
#include "iimaudio/WaveFile.h"
#include <vector>

using namespace iimaudio;
int main()
{
	// Prepare parameters for audio capture
	audio_params_t params {
			sampling_rate_t::rate_44kHz,			// Sample at 44100 Hz
			sampling_format_t::format_16bit_signed,	// Each sample will be 16bit signed integer
			1										// Capture single channel
	};

	// Initalize the capture device
	CaptureDevice device (params);

	// Prepare wave file
	WaveFile wav("capture.wav", device.get_params());

	//! Buffer for storing samples
	std::vector<int16_t> buffer(16384,0);
	//! Number of captured samples
	size_t captured;
	//! Error core returned from the device
	return_type_t error;

	// Start the capture
	error = device.start_capture();
	// Did it succeed?
	if (error != return_type_t::ok) {
		log[log_level::fatal] << "Error starting capture: " << get_error_string(error) << "\n";
		return 1;
	}
	// Capture is started
	log[log_level::info] << "Capture started\n";
	// Capture samples for a while
	for (int i = 0; i< 30;++i) {
		// Capture samples
		captured = device.capture_data(buffer,error);
		// Did it succeed?
		if (captured > 0) {
			log[log_level::info] << "Captured " << captured << "samples\n";
			// Store captured samples to a wav file
			wav.store_data(buffer,captured);
		} else {
			log[log_level::fatal] << "Error during capture: " << get_error_string(error) << "\n";
			break;
		}
	}
}

