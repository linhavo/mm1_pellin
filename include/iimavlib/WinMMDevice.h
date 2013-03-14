/**
 * @file 	WinMMDevice.h
 *
 * @date 	20.1.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 * This file defines backend driver for WinMM API
 */

#ifndef WINMMDEVICE_H_
#define WINMMDEVICE_H_

#include "AudioTypes.h"
#include "GenericDevice.h"
#include "PlatformDefs.h"
#include <windows.h>
#include <mmsystem.h>
#include <mutex>
#include <map>
//#include <string>
//#include <alsa/asoundlib.h>
//#include <vector>
//#include <algorithm>

namespace iimavlib {


class EXPORT WinMMDevice:public GenericDevice {
public:
	typedef UINT audio_id_t;
	WinMMDevice(action_type_t action, audio_id_t id, const audio_params_t& params);
	virtual ~WinMMDevice();
	static audio_id_t default_device();

	error_type_t do_start_capture();

	size_t 	do_capture_data(uint8_t* data_start, size_t data_size, error_type_t& error_code);

	error_type_t do_set_buffers(uint16_t count, uint32_t samples);

	error_type_t do_fill_buffer(const uint8_t* data_start, size_t data_size);

	error_type_t do_start_playback();

	error_type_t do_update(size_t delay = 10);
	audio_params_t do_get_params() const {return params_;}

	void store_data(WAVEHDR& hdr);
	static std::map<audio_id_t, audio_info_t> do_enumerate_capture_devices();
	static std::map<audio_id_t, audio_info_t> do_enumerate_playback_devices();
private:
	action_type_t 		action_;
	audio_id_t			id_;
	audio_params_t		params_;
	DWORD				sampling_rate_;
	WORD				bps_;
	HWAVEIN				in_handle;
	HWAVEOUT			out_handle;
	std::vector<WAVEHDR>buffers;
	circular_buffer_t<uint8_t>
						private_buffer_;
	std::mutex			buffer_lock_;

	size_t buffer_length;
	//size_t buffer_count;
	static const size_t capture_buffer_length 	= 2048;
	static const size_t capture_buffer_count 	= 8;
	
	std::vector<WAVEHDR*> empty_buffers;

//	snd_pcm_t			*handle_;
//	snd_pcm_stream_t	stream_type_;
//	size_t				sample_size_;
//
//	std::vector<audio_buffer_t>	buffers;
//	size_t				first_empty_buffer;
//	size_t				first_full_buffer;
//
	void init_capture(WAVEFORMATEX& fmt);
	void init_in_buffer(WAVEHDR& hdr);
	void init_out_buffer(WAVEHDR& hdr);
	void init_playback(WAVEFORMATEX& fmt);
	static bool check_call(MMRESULT res, std::string message);
	static void throw_call(bool res, std::string message);
	static void throw_call(MMRESULT res, std::string message);
};

}
#endif /* WINMMDEVICE_H_ */
