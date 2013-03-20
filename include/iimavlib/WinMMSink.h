/**
 * @file 	WinMMSink.h
 *
 * @date 	11.3.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 * This file defines WinMM-based sink filter
 */

#ifndef WinMMSINK_H_
#define WinMMSINK_H_

#include "AudioSink.h"
#include "PlatformDefs.h"
#include <mutex>
#include <deque>
#include <condition_variable>
#include <windows.h>
#include <mmsystem.h>

namespace iimavlib {
struct win_mm_buffer_t {
	audio_buffer_t	buffer_;
	WAVEHDR			hdr_;
	bool			prepared_;
	win_mm_buffer_t():prepared_(false) {}
};

class EXPORT WinMMSink: public AudioSink {
public:
	typedef UINT audio_id_t;
	static audio_id_t default_device();

	WinMMSink(const pAudioFilter& child_,
				audio_id_t id = default_device());

	WinMMSink(const pAudioFilter& child_,
				const audio_params_t& params=audio_params_t(),
				audio_id_t id = default_device());

	~WinMMSink();
	
	void enqueue_buffer(win_mm_buffer_t& buffer);

private:
	// AudioSink API
	error_type_t do_run();
	virtual void do_set_buffers(size_t count, size_t size);

	// Private methods
	void init_buffers();
	bool winmm_prepare_buffer(win_mm_buffer_t& buffer);
	bool winmm_unprepare_buffer(win_mm_buffer_t& buffer);
	bool winmm_start_playback();
	bool winmm_open_device();
	bool winmm_close_device();
	bool fill_buffer(win_mm_buffer_t& buffer);
	
	static bool check_call(MMRESULT res, std::string message);
	static void throw_call(bool res, std::string message);
	static void throw_call(MMRESULT res, std::string message);

	HWAVEOUT			out_handle;
	audio_id_t			id_;
	const audio_params_t params_;
	size_t				buffer_count_;
	size_t				buffer_size_;
	std::vector<win_mm_buffer_t> 
						buffers_;
	std::mutex			buffer_lock_;
	std::deque<win_mm_buffer_t*>
						buffer_queue_;
	std::mutex			queue_lock_;
	std::condition_variable
						queue_notify_;
	bool				flushing_;
	size_t				prepared_buffers_;


};


}
#endif /* WinMMSINK_H_ */
