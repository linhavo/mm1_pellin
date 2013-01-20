/*
 * WinMMDevice.h
 *
 *  Created on: 20.1.2013
 *      Author: neneko
 */

#ifndef WINMMDEVICE_H_
#define WINMMDEVICE_H_

#include "AudioTypes.h"
#include "GenericDevice.h"
#include <windows.h>
#include <mmsystem.h>
//#include <string>
//#include <alsa/asoundlib.h>
//#include <vector>
//#include <algorithm>

namespace iimaudio {


class WinMMDevice:public GenericDevice {
public:
	typedef UINT audio_id_t;
	WinMMDevice(action_type_t action, audio_id_t id, const audio_params_t& params);
	virtual ~WinMMDevice();
	static audio_id_t default_device();

	return_type_t do_start_capture() {}

	size_t 	do_capture_data(uint8_t* data_start, size_t data_size, return_type_t& error_code) {}

	return_type_t do_set_buffers(uint16_t count, uint32_t samples) {}

	return_type_t do_fill_buffer(const uint8_t* data_start, size_t data_size) {}

	return_type_t do_start_playback() {}

	return_type_t do_update(size_t delay = 10) {}
	audio_params_t do_get_params() const {}

private:
	action_type_t 		action_;
	audio_id_t			id_;
	audio_params_t		params_;
	DWORD				sampling_rate_;
	WORD				bps_;
	HWAVEIN				in_handle;
	std::vector<WAVEHDR>buffers;

	static const size_t buffer_length 	= 1024;
	static const size_t buffer_count 	= 16;

//	snd_pcm_t			*handle_;
//	snd_pcm_stream_t	stream_type_;
//	size_t				sample_size_;
//
//	std::vector<audio_buffer_t>	buffers;
//	size_t				first_empty_buffer;
//	size_t				first_full_buffer;
//
	void init_capture(WAVEFORMATEX& fmt);
	void init_buffer(WAVEHDR& hdr);
	static bool check_call(MMRESULT res, std::string message);
	static void throw_call(bool res, std::string message);
	static void throw_call(MMRESULT res, std::string message);
};

}
#endif /* WINMMDEVICE_H_ */
