/*
 * GenericDevice.h
 *
 *  Created on: 20.1.2013
 *      Author: neneko
 */

#ifndef GENERICDEVICE_H_
#define GENERICDEVICE_H_
#include "AudioTypes.h"
namespace iimaudio {
class GenericDevice
{
public:
	GenericDevice() {}
	virtual ~GenericDevice() {}

	virtual return_type_t do_start_capture() = 0;


	virtual size_t 	do_capture_data(uint8_t* data_start, size_t data_size, return_type_t& error_code) = 0;

	virtual return_type_t do_set_buffers(uint16_t count, uint32_t samples) = 0;

	virtual return_type_t do_fill_buffer(const uint8_t* data_start, size_t data_size) = 0;

	virtual return_type_t do_start_playback() = 0;

	virtual return_type_t do_update(size_t delay = 10) = 0;
	virtual audio_params_t do_get_params() const = 0;

};
}

#endif /* GENERICDEVICE_H_ */
