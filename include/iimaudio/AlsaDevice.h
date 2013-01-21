/**
 * @file 	AlsaDevice.h
 *
 * @date 	19.1.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 * This file defines Alsa backend
 */

#ifndef ALSADEVICE_H_
#define ALSADEVICE_H_

#include "AudioTypes.h"
#include "GenericDevice.h"
#include <string>
#include <alsa/asoundlib.h>
#include <vector>
#include <map>
#include <algorithm>

namespace iimaudio {


class AlsaDevice:public GenericDevice {
public:
	typedef std::string audio_id_t;
	AlsaDevice(action_type_t action, audio_id_t id, const audio_params_t& params);
	virtual ~AlsaDevice();
	static audio_id_t default_device();

	return_type_t do_start_capture();

	size_t 	do_capture_data(uint8_t* data_start, size_t data_size, return_type_t& error_code);

	return_type_t do_set_buffers(uint16_t count, uint32_t samples);

	return_type_t do_fill_buffer(const uint8_t* data_start, size_t data_size);

	return_type_t do_start_playback();

	return_type_t do_update(size_t delay = 10);
	audio_params_t do_get_params() const;

	static std::map<audio_id_t, audio_info_t> do_enumerate_capture_devices();
	static std::map<audio_id_t, audio_info_t> do_enumerate_playback_devices();
private:
	action_type_t 		action_;
	audio_id_t			id_;
	audio_params_t		params_;
	unsigned int		sampling_rate_;
	snd_pcm_t			*handle_;
	snd_pcm_stream_t	stream_type_;
	size_t				sample_size_;

	std::vector<audio_buffer_t>	buffers;
	size_t				first_empty_buffer;
	size_t				first_full_buffer;

	static bool check_call(int res, std::string message);
	static void throw_call(bool res, std::string message);
	static void throw_call(int res, std::string message);

	static void enumerate_hw_devices(std::map<audio_id_t, audio_info_t>&map_, snd_pcm_stream_t type_);

};

}

#endif /* ALSADEVICE_H_ */
