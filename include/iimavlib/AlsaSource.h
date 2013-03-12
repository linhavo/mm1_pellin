/*
 * AlsaSource.h
 *
 *  Created on: 12.3.2013
 *      Author: neneko
 */

#ifndef ALSASOURCE_H_
#define ALSASOURCE_H_

#include "AudioFilter.h"
#include "AlsaDevice.h"
namespace iimavlib {
class AlsaSource: public AudioFilter {
public:
	AlsaSource(const audio_params_t& params=audio_params_t(),
				AlsaDevice::audio_id_t id = AlsaDevice::default_device());
	virtual ~AlsaSource();
private:
	virtual error_type_t do_process(audio_buffer_t& buffer);
	virtual audio_params_t do_get_params() const;
	AlsaDevice device_;
};

}
#endif /* ALSASOURCE_H_ */
