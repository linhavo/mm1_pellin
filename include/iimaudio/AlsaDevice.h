/*
 * AlsaDevice.h
 *
 *  Created on: 19.1.2013
 *      Author: neneko
 */

#ifndef ALSADEVICE_H_
#define ALSADEVICE_H_

#include "AudioTypes.h"
#include <string>
#include <alsa/asoundlib.h>
#include <vector>

namespace iimaudio {

class AlsaDevice {
public:
	typedef std::string audio_id_t;
	AlsaDevice(action_type_t action, audio_id_t id, const audio_params_t& params);
	~AlsaDevice();


	return_type_t start_capture();

	template<typename T>
	size_t capture_data(std::vector<T>& buffer, return_type_t& error_code);

	static audio_id_t default_device();

	audio_params_t get_params();


private:
	action_type_t 		action_;
	audio_id_t			id_;
	audio_params_t		params_;
	unsigned int		sampling_rate_;
	snd_pcm_t			*handle_;
	snd_pcm_stream_t	stream_type_;
	size_t				sample_size_;

	static bool check_call(int res, std::string message);
	static void throw_call(bool res, std::string message);
	static void throw_call(int res, std::string message);




};


template<typename T>
size_t AlsaDevice::capture_data(std::vector<T>& buffer, return_type_t& error_code)
{
	int ret;
	const unsigned long buffer_size = static_cast<unsigned long>(buffer.size()*sizeof(T)/sample_size_);
	if (!check_call(ret = snd_pcm_readi(handle_,reinterpret_cast<void*>(&buffer[0]),
						buffer_size),
					"Failed to read data"))
	{
		if (ret == -EPIPE) {
			error_code = return_type_t::xrun;
		} else if (ret <0) error_code = return_type_t::failed;
		return 0;
	}
	error_code = return_type_t::ok;
	return static_cast<size_t>(ret);

}

}



#endif /* ALSADEVICE_H_ */
