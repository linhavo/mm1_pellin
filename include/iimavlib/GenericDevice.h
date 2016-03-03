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
#include "PlatformDefs.h"
namespace iimavlib {
/*!
 * Generic autio device supporting input/output or both operations
 */
class EXPORT GenericDevice
{
public:
#ifdef MODERN_COMPILER
	/*!
	 * Default ctor
	 */
	GenericDevice() = default;
	/*!
	 * Default dtor
	 */
	virtual ~GenericDevice() = default;
#else
	virtual ~GenericDevice() {}
#endif
	/*!
	 * @brief Starts the capture
	 *
	 * @return Returns ok if the capture was started successfully.
	 */
	virtual error_type_t do_start_capture() = 0;

	/*!
	 * @brief Read data and store them to a buffer
	 * @param data_start Address of beginning of the buffer
	 * @param data_size Length of the buffer (in bytes)
	 * @param error_code [out] Error code
	 * @return Number of @em samples read
	 */
	virtual size_t 	do_capture_data(audio_sample_t* data_start, size_t data_size, error_type_t& error_code) = 0;

	/*!
	 * Set count and size of internal buffers used
	 * @param count Number of buffers
	 * @param samples Size of each buffer in samples
	 * @return Returns error_type_t::ok if the call was successful.
	 */
	virtual error_type_t do_set_buffers(uint16_t count, uint32_t samples) = 0;

	/*!
	 * Fill a provided buffer with samples
	 * @param data_start
	 * @param data_size
	 * @return Returns error_type_t::ok if the call was successful.
	 */
	virtual error_type_t do_fill_buffer(const audio_sample_t* data_start, size_t data_size) = 0;

	/*!
	 * Start playback
	 * @return Returns error_type_t::ok if the call was successful.
	 */
	virtual error_type_t do_start_playback() = 0;

	/*!
	 * Updates internal state (loads data from source into buffers, etc.)
	 * @param delay Time in milliseconds to wait for new data
	 * @return Returns error_type_t::ok if the call was successful.
	 */
	virtual error_type_t do_update(size_t delay = 10) = 0;

	/*!
	 * Get configures parameters of a device or the pipeline
	 * @return Audio parameters of the device/pipeline
	 */
	virtual audio_params_t do_get_params() const = 0;

};
}

#endif /* GENERICDEVICE_H_ */
