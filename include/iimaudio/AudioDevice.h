/*
 * AudioDevice.h
 *
 *  Created on: 19.1.2013
 *      Author: neneko
 */

#ifndef AUDIODEVICE_H_
#define AUDIODEVICE_H_
#include "AudioTypes.h"
#include "AudioPlatform.h"

namespace iimaudio {

template<action_type_t action = action_type_t::action_capture, class Device = PlatformDevice>
class AudioDevice
{
public:
	AudioDevice();
	AudioDevice(const audio_params_t& params, audio_id device_id = default_device());

	virtual ~AudioDevice() {}

	return_type_t start_capture();

	static audio_id default_device();

	template<typename T>
	size_t capture_data(std::vector<T>& buffer, return_type_t& error_code);

	audio_params_t get_params();

private:
	typedef Device NDevice;
	static const action_type_t action_ = action;
	NDevice native_device_;

};


/* Implementation of AudioDevice */
template<action_type_t action, class Device>
AudioDevice<action, Device>::AudioDevice():
		AudioDevice<action, Device>(default_device(),audio_params_t()) {}

template<action_type_t action, class Device>
AudioDevice<action, Device>::AudioDevice(const audio_params_t& params,
		audio_id device_id):
		native_device_(action_, device_id, params)
{

}

template<action_type_t action, class Device>
audio_id AudioDevice<action, Device>::default_device()
{
	return NDevice::default_device();
}

template<action_type_t action, class Device>
return_type_t AudioDevice<action, Device>::start_capture()
{
#ifndef NDEBUG
	static_assert(action_ == action_type_t::action_capture,"");
#else
	if (action_ != action_type_t::action_capture) return return_type_t::failed;
#endif
	return return_type_t::ok;
}

template<action_type_t action, class Device>
template<typename T>
	size_t AudioDevice<action, Device>::capture_data(std::vector<T>& buffer, return_type_t& error_code)
{
	return native_device_.capture_data(buffer,error_code);
}

template<action_type_t action, class Device>
audio_params_t AudioDevice<action, Device>::get_params()
{
	return native_device_.get_params();
}

}

#endif /* AUDIODEVICE_H_ */
