/**
 * @file 	AudioDevice.h
 *
 * @date 	19.1.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 * This file defines the user API.
 */

#ifndef AUDIODEVICE_H_
#define AUDIODEVICE_H_
#include "AudioTypes.h"
#include "AudioPlatform.h"
#include "AudioPolicies.h"
#include <array>
#include <map>
namespace iimavlib {

/*!
 * @class AudioDevice
 * @brief Entrypoint for the public API
 *
 * @tparam action Requested action, either capture or playback
 * @tparam Threading Threading Policy
 * @tparam Device Backend driver
 */
template<action_type_t action = action_type_t::action_capture, class Threading = SingleThreaded, class Device = PlatformDevice>
class AudioDevice:private Device,private Threading
{
public:
	typedef typename Device::audio_id_t audio_id;
	typedef typename Threading::lock_t lock_t;
	/*!
	 * @brief Default constructor
	 *
	 * Constructs AudioDevice using default parameters and default device
	 */
	AudioDevice();

	AudioDevice(const audio_params_t& params, audio_id device_id = Device::default_device());
	virtual ~AudioDevice() {}

	/*!
	 * @brief Starts the device
	 *
	 * Starts capture or playback, depending on the @em action template parameter
	 */
	error_type_t start();

	/*!
	 * @brief [capture] Starts capture
	 *
	 */
	 error_type_t start_capture();

	/*!
	 * @brief [capture] Reads data from the device to a vector
	 *
	 * Works with vector of any type.
	 * The underlying storage is considered as a continuous memory block.
	 * The method will return number of samples read (NOT number of bytes).
	 * It can return less samples than can fit into the buffer when there is not enough
	 * data available.
	 * When there are no data available, the method returns 0 and sets @em error_code
	 * to @em buffer_empty. Application should not consider this condition as an error.
	 *
	 * @tparam 	T 		type of data in the vector
	 * @param 	buffer	vector to store data to
	 * @param	error_code	When an error occurs, the error code will be stored here
	 * @return 			Returns number of captured samples, or 0 when an error occurred.
	 */
	 template<typename T>
	size_t capture_data(std::vector<T>& buffer, error_type_t& error_code);
	/*!
	 * @brief [capture] Reads data from the device to an std::array
	 *
	 * Works with array of any type.
	 * The underlying storage is considered as a continuous memory block.
	 * The method will return number of samples read (NOT number of bytes).
	 * It can return less samples than can fit into the buffer when there is not enough
	 * data available.
	 * When there are no data available, the method returns 0 and sets @em error_code
	 * to @em buffer_empty. Application should not consider this condition as an error.
	 *
	 * @tparam 	T		type of data in the vector
	 * @tparam	S		size of the array
	 * @param 	buffer	vector to store data to
	 * @param	error_code	When an error occurs, the error code will be stored here
	 * @return 			Returns number of captured samples, or 0 when an error occurred.
		 */
	 template<typename T, std::size_t S>
	size_t capture_data(std::array<T,S>& buffer, error_type_t& error_code);
	/*!
	 * @brief [capture] Reads data from the device to a raw memory area
	 *
	 * Works with a pointer of any type.
	 * The underlying storage is considered as a continuous memory block.
	 * The method will return number of samples read (NOT number of bytes).
	 * It can return less samples than can fit into the buffer when there is not enough
	 * data available.
	 * When there are no data available, the method returns 0 and sets @em error_code
	 * to @em buffer_empty. Application should not consider this condition as an error.
	 *
	 * @tparam 	T 		type of data in the vector
	 * @param 	raw_data	vector to store data to
	 * @param	data_size	size of the buffer
	 * @param	error_code	When an error occurs, the error code will be stored here
	 * @return 			Returns number of captured samples, or 0 when an error occurred.
	 */
	 template<typename T>
	size_t capture_data(T* raw_data, std::size_t data_size, error_type_t& error_code);

	/*!
	 * @brief [playback] Sets number and size of buffers used for playback
	 *
	 * @param	count	Number of buffers to allocate
	 * @param	samples	Number of samples each of the buffers should store
	 * @return			Returns ok if buffers were correctly allocated
	 */
	 error_type_t set_buffers(uint16_t count, uint32_t samples);


	 template<typename T>
	error_type_t fill_buffer(const std::vector<T>& data);

	 error_type_t start_playback();

	 error_type_t update(size_t delay = 10);
	 audio_params_t get_params() const;

	 static std::map<audio_id, audio_info_t> enumerate_devices();
	 static std::map<audio_id, audio_info_t> enumerate_capture_devices();
	 static std::map<audio_id, audio_info_t> enumerate_playback_devices();
};



/* Implementation of AudioDevice */
template<action_type_t action, class Threading, class Device>
AudioDevice<action, Threading, Device>::AudioDevice():
//#ifndef _WIN32
//		AudioDevice<action, Threading, Device>(audio_params_t(),Device::default_device()) {}
//#else // Visual studio compiler doesn't support delegating constructors
	Device(action, Device::default_device(), audio_params_t()) {}
//#endif

template<action_type_t action, class Threading, class Device>
AudioDevice<action, Threading, Device>::AudioDevice(const audio_params_t& params,
		audio_id device_id):
		Device(action, device_id, params)
{

}

template<action_type_t action, class Threading, class Device>
error_type_t AudioDevice<action, Threading, Device>::start()
{
	switch(action) {
		case action_type_t::action_capture: return start_capture();
		case action_type_t::action_playback: return start_playback();
		default:
			throw std::runtime_error("Unsupported action");
	}
}

template<action_type_t action, class Threading, class Device>
error_type_t AudioDevice<action, Threading, Device>::start_capture()
{
	lock_t lock = Threading::lock_instance();
	return Device::do_start_capture();
}

template<action_type_t action, class Threading, class Device>
error_type_t AudioDevice<action, Threading, Device>::set_buffers(uint16_t count, uint32_t samples)
{
	lock_t lock = Threading::lock_instance();
	return Device::do_set_buffers(count, samples);
}

template<action_type_t action, class Threading, class Device>
error_type_t AudioDevice<action, Threading, Device>::start_playback()
{
	lock_t lock = Threading::lock_instance();
	return Device::do_start_playback();
}

template<action_type_t action, class Threading, class Device>
error_type_t AudioDevice<action, Threading, Device>::update(size_t delay)
{
	lock_t lock = Threading::lock_instance();
	return Device::do_update(delay);
}

template<action_type_t action, class Threading, class Device>
audio_params_t AudioDevice<action, Threading, Device>::get_params() const
{
	lock_t lock = Threading::lock_instance();
	return Device::do_get_params();
}


template<action_type_t action, class Threading, class Device>
template<typename T>
error_type_t AudioDevice<action, Threading, Device>::fill_buffer(const std::vector<T>& data)
{
	lock_t lock = Threading::lock_instance();
	return Device::do_fill_buffer(reinterpret_cast<const uint8_t*>(&data[0]),data.size()*sizeof(T));

}

template<action_type_t action, class Threading, class Device>
template<typename T>
size_t AudioDevice<action, Threading, Device>::capture_data(std::vector<T>& buffer, error_type_t& error_code)
{
	lock_t lock = Threading::lock_instance();
	return Device::do_capture_data(reinterpret_cast<uint8_t*>(&buffer[0]),buffer.size()*sizeof(T),error_code);
}
template<action_type_t action, class Threading, class Device>
template<typename T, std::size_t S>
size_t AudioDevice<action, Threading, Device>::capture_data(std::array<T,S>& buffer, error_type_t& error_code)
{
	lock_t lock = Threading::lock_instance();
	return Device::do_capture_data(reinterpret_cast<uint8_t*>(&buffer[0]),S*sizeof(T),error_code);
}
template<action_type_t action, class Threading, class Device>
template<typename T>
size_t AudioDevice<action, Threading, Device>::capture_data(T* raw_data, std::size_t data_size, error_type_t& error_code)
{
	lock_t lock = Threading::lock_instance();
	return Device::do_capture_data(reinterpret_cast<uint8_t*>(raw_data),data_size*sizeof(T),error_code);
}

template<action_type_t action, class Threading, class Device>
std::map<typename AudioDevice<action, Threading, Device>::audio_id, audio_info_t> AudioDevice<action, Threading, Device>::enumerate_devices()
{
	switch(action) {
		case action_type_t::action_capture: return enumerate_capture_devices();
		case action_type_t::action_playback: return enumerate_playback_devices();
		default:
			throw std::runtime_error("Unsupported action");
	}
}


template<action_type_t action, class Threading, class Device>
std::map<typename AudioDevice<action, Threading, Device>::audio_id, audio_info_t> AudioDevice<action, Threading, Device>::enumerate_capture_devices()
{
	return Device::do_enumerate_capture_devices();
}

template<action_type_t action, class Threading, class Device>
std::map<typename AudioDevice<action, Threading, Device>::audio_id, audio_info_t> AudioDevice<action, Threading, Device>::enumerate_playback_devices()
{
	return Device::do_enumerate_playback_devices();
}


}

#endif /* AUDIODEVICE_H_ */
