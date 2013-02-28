/**
 * @file 	GenericDevice.h
 *
 * @date 	20.1.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 * Interface for backend drivers
 */

#ifndef GENERICDEVICE_H_
#define GENERICDEVICE_H_
#include "AudioTypes.h"
namespace iimavlib {
class GenericDevice
{
public:
	GenericDevice() {}
	virtual ~GenericDevice() {}

	/*!
	 * @brief Starts the capture
	 *
	 * @return Returns ok if the capture was started successfully.
	 */
	virtual error_type_t do_start_capture() = 0;

	/*!
	 * @brief Read data and store the to a buffer
	 * @param data_start Address of beginning of the buffer
	 * @param data_size Length of the buffer (in bytes)
	 * @param error_code [out] Error code
	 * @return Number of @em samples read
	 */
	virtual size_t 	do_capture_data(uint8_t* data_start, size_t data_size, error_type_t& error_code) = 0;

	virtual error_type_t do_set_buffers(uint16_t count, uint32_t samples) = 0;

	virtual error_type_t do_fill_buffer(const uint8_t* data_start, size_t data_size) = 0;

	virtual error_type_t do_start_playback() = 0;

	virtual error_type_t do_update(size_t delay = 10) = 0;
	virtual audio_params_t do_get_params() const = 0;

};
}

#endif /* GENERICDEVICE_H_ */
