/*
 * WinMMSink.h
 *
 *  Created on: 11.3.2013
 *      Author: neneko
 */

#ifndef WinMMSINK_H_
#define WinMMSINK_H_

#include "WinMMDevice.h"
#include "AudioSink.h"
namespace iimavlib {

class EXPORT WinMMSink: public AudioSink {
public:
	WinMMSink(const pAudioFilter& child_,
				WinMMDevice::audio_id_t id = WinMMDevice::default_device());

	WinMMSink(const pAudioFilter& child_,
				const audio_params_t& params=audio_params_t(),
				WinMMDevice::audio_id_t id = WinMMDevice::default_device());

	~WinMMSink();

private:
	error_type_t do_run();
	void init_buffers();
	virtual void do_set_buffers(size_t count, size_t size);
	WinMMDevice device_;
	const audio_params_t params_;
	size_t buffer_count_;
	size_t buffer_size_;
	audio_buffer_t buffer_;


};


}
#endif /* WinMMSINK_H_ */
