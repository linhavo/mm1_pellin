/*
 * AlsaSink.h
 *
 *  Created on: 11.3.2013
 *      Author: neneko
 */

#ifndef ALSASINK_H_
#define ALSASINK_H_

#include "AlsaDevice.h"
#include "AudioSink.h"
namespace iimavlib {

class AlsaSink: public AudioSink {
public:
	AlsaSink(const pAudioFilter& child_,
				AlsaDevice::audio_id_t id = AlsaDevice::default_device());

	AlsaSink(const pAudioFilter& child_,
				const audio_params_t& params=audio_params_t(),
				AlsaDevice::audio_id_t id = AlsaDevice::default_device());

	~AlsaSink();

private:
	error_type_t do_run();
	void init_buffers();
	virtual void do_set_buffers(size_t count, size_t size);
	AlsaDevice device_;
	const audio_params_t params_;
	size_t buffer_count_;
	size_t buffer_size_;
	audio_buffer_t buffer_;


};


}
#endif /* ALSASINK_H_ */
