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
#include <algorithm>

namespace iimaudio {


class AlsaDevice {
public:
	typedef std::string audio_id_t;
	AlsaDevice(action_type_t action, audio_id_t id, const audio_params_t& params);
	virtual ~AlsaDevice();
	static audio_id_t default_device();

	return_type_t start_capture();

	template<typename T>
	size_t capture_data(std::vector<T>& buffer, return_type_t& error_code);

	return_type_t set_buffers(uint16_t count, uint32_t samples);
	template<typename T>
	return_type_t fill_buffer(const std::vector<T>& data);

	return_type_t start_playback();

	return_type_t update(size_t delay = 10);
	audio_params_t get_params();

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

template<typename T>
return_type_t AlsaDevice::fill_buffer(const std::vector<T>& data)
{
	if (first_empty_buffer >= buffers.size()) return return_type_t::invalid;
	audio_buffer_t &buf = buffers[first_empty_buffer];
	if (!buf.empty) return return_type_t::buffer_full;
	size_t copy_bytes = std::min(buf.data.size(),data.size()*sizeof(T));
	std::copy_n(reinterpret_cast<const uint8_t*>(&data[0]),copy_bytes,buf.data.begin());
	buf.position = 0;
	buf.empty = false;
	if (buffers[first_full_buffer].empty) first_full_buffer = first_empty_buffer;
	first_empty_buffer = (first_empty_buffer+1)%buffers.size();
	return return_type_t::ok;
}
}



#endif /* ALSADEVICE_H_ */
