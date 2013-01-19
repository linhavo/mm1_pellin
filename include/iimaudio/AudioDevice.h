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
class AudioDevice:public Device
{
public:
	typedef typename Device::audio_id_t audio_id;
	AudioDevice();
	AudioDevice(const audio_params_t& params, audio_id device_id = Device::default_device());
	virtual ~AudioDevice() {}
};



/* Implementation of AudioDevice */
template<action_type_t action, class Device>
AudioDevice<action, Device>::AudioDevice():
		AudioDevice<action, Device>(audio_params_t(),Device::default_device()) {}

template<action_type_t action, class Device>
AudioDevice<action, Device>::AudioDevice(const audio_params_t& params,
		audio_id device_id):
		Device(action, device_id, params)
{

}
}

#endif /* AUDIODEVICE_H_ */
