/**
 * @file 	WinMMSink.cpp
 *
 * @date 	11.3.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 */

#include "iimavlib/WinMMSink.h"
#include "iimavlib/Utils.h"
#include <algorithm>
#pragma comment(lib, "winmm.lib")
namespace iimavlib {
namespace {

void CALLBACK win_mm_device_playback_callback (HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
	if (uMsg == WOM_DONE) {	
		WinMMSink *winmm = reinterpret_cast<WinMMSink*>(dwInstance);
		WAVEHDR *hdr = reinterpret_cast<WAVEHDR*>(dwParam1);
		assert(hdr->dwUser);
		win_mm_buffer_t& buffer = *reinterpret_cast<win_mm_buffer_t*>(hdr->dwUser);
		//logger[log_level::debug] << "Filling in next buffer 0x" << std::hex << hdr;
		winmm->enqueue_buffer(buffer);
	}
}
}

WinMMSink::audio_id_t WinMMSink::default_device()
{
	return WAVE_MAPPER;
}
WinMMSink::WinMMSink(const pAudioFilter& child_, const audio_params_t& params, WinMMSink::audio_id_t id):
		AudioSink(child_),id_(id),
		params_(params),
		buffer_count_(4),buffer_size_(1024),flushing_(false),prepared_buffers_(0)
{
	winmm_open_device();
	init_buffers();
}

WinMMSink::WinMMSink(const pAudioFilter& child_, WinMMSink::audio_id_t id):
		AudioSink(child_),id_(id),
		params_(get_params()),
		buffer_count_(4),buffer_size_(1024),flushing_(false),prepared_buffers_(0)
{
	winmm_open_device();
	init_buffers();
}

bool WinMMSink::winmm_open_device()
{
	size_t sampling_rate_ 		= convert_rate_to_int(params_.rate);
	size_t bps_ 				= params_.sample_size()* 8;

	WAVEFORMATEX fmt;
	fmt.wFormatTag		= WAVE_FORMAT_PCM;
	fmt.nChannels		= static_cast<WORD>(2/*number_of_channels*/);
	fmt.nSamplesPerSec	= static_cast<DWORD>(sampling_rate_);
	fmt.nAvgBytesPerSec	= static_cast<DWORD>(sampling_rate_*4/*bps_/8*/);
	fmt.nBlockAlign		= static_cast<WORD>(4);
	fmt.wBitsPerSample	= static_cast<WORD>(16);
	fmt.cbSize			= 0;
	throw_call(waveOutOpen(&out_handle,id_,&fmt,
		reinterpret_cast<DWORD_PTR>(&win_mm_device_playback_callback),
		reinterpret_cast<DWORD_PTR>(this),
		WAVE_FORMAT_DIRECT|CALLBACK_FUNCTION),
		"Failed to open wave out devce!");
	logger[log_level::debug] << "WinMM Device opened";
	waveOutReset(out_handle);	
	waveOutPause(out_handle);
	return true;
}
bool WinMMSink::winmm_prepare_buffer(win_mm_buffer_t& buffer) 
{
	if (buffer.prepared_) return true;
	buffer.hdr_.lpData = reinterpret_cast<LPSTR>(&(buffer.buffer_.data[0]));
	//buffer.hdr_.dwBufferLength = static_cast<DWORD>(buffer.buffer_.data.size());
	buffer.hdr_.dwBufferLength = static_cast<DWORD>(buffer.buffer_.valid_samples*params_.sample_size());
	buffer.hdr_.dwBytesRecorded = 0;
	buffer.hdr_.dwUser	= reinterpret_cast<DWORD_PTR>(&buffer);
	buffer.hdr_.dwFlags = 0;
	buffer.hdr_.dwLoops = 0;

	if (check_call(waveOutPrepareHeader(out_handle, &buffer.hdr_, sizeof(WAVEHDR)),"Failed to prepare buffer")) {
		buffer.prepared_ = true;
		prepared_buffers_++;
		return true;
	}
	return false;	
}

bool WinMMSink::winmm_unprepare_buffer(win_mm_buffer_t& buffer) 
{
	if (!buffer.prepared_) return true;
	assert(prepared_buffers_ > 0);
	if (check_call(waveOutUnprepareHeader(out_handle, &buffer.hdr_, sizeof(WAVEHDR)),"Failed to unprepare buffer")) {
		buffer.prepared_ = false;
		prepared_buffers_--;
		return true;
	}
	return false;	
}
bool WinMMSink::winmm_start_playback()
{
	return check_call(waveOutRestart(out_handle),"Failed to start playback");
}
bool WinMMSink::winmm_close_device()
{
	waveOutReset(out_handle);
	{
		std::unique_lock<std::mutex> lock(buffer_lock_);
		for (win_mm_buffer_t& buffer:buffers_) winmm_unprepare_buffer(buffer);
	}
	if(check_call(waveOutClose(out_handle),"Failed to close the device")) {
		logger[log_level::debug] << "WinMM device closed";
		return true;
	}
	return false;
}
void WinMMSink::init_buffers()
{
	std::unique_lock<std::mutex> lock(buffer_lock_);
	for (win_mm_buffer_t& buffer:buffers_) {
		winmm_unprepare_buffer(buffer);
	}
	buffers_.resize(buffer_count_);
	for (win_mm_buffer_t& buffer:buffers_) {
		buffer.buffer_.data.resize(buffer_size_,0);
		buffer.buffer_.params = params_;
		buffer.buffer_.valid_samples=buffer_size_;
		winmm_prepare_buffer(buffer);
	}
	logger[log_level::debug] << "Prepared " <<  buffer_count_ << " buffers";
}

WinMMSink::~WinMMSink()
{

}
void WinMMSink::enqueue_buffer(win_mm_buffer_t& buffer)
{
	std::unique_lock<std::mutex> lock(queue_lock_);
	buffer_queue_.push_back(&buffer);
	queue_notify_.notify_one();
}
bool WinMMSink::fill_buffer(win_mm_buffer_t& buffer)
{
	assert(buffer.prepared_);
	std::unique_lock<std::mutex> lock(buffer_lock_);
	while(true) {
		if (!still_running()) return false;
		buffer.buffer_.valid_samples=buffer_size_;
		if (process(buffer.buffer_)!=error_type_t::ok) return false;
		if (buffer.buffer_.valid_samples) {
			//if (buffer.buffer_.valid_samples!=buffer_size_) {
			winmm_unprepare_buffer(buffer);
			winmm_prepare_buffer(buffer);
			//buffer.hdr_.dwBufferLength=buffer.buffer_.valid_samples*params_.sample_size();
			//}
			break;
		}
	}
	return check_call(waveOutWrite(out_handle,&buffer.hdr_,sizeof(WAVEHDR)),"Failed to write buffer");
}
error_type_t WinMMSink::do_run()
{
	for (win_mm_buffer_t& buffer:buffers_) {
		if (!fill_buffer(buffer)) stop();
	}
	
	if (!winmm_start_playback()) stop();
	while (still_running()) {
		if (flushing_) {
			if (prepared_buffers_ == 0) stop();
		}
		win_mm_buffer_t* buffer = nullptr;
		{
			std::unique_lock<std::mutex> lock(queue_lock_);
			if (!flushing_) queue_notify_.wait(lock);
			if (!buffer_queue_.empty()) {
				buffer= buffer_queue_.front();
				buffer_queue_.pop_front();
			}
		}
		if (buffer!=nullptr) {
			if (!flushing_) {
				if (!fill_buffer(*buffer)) {
					logger[log_level::debug] << "End of source, waiting for remaining buffers.";
					flushing_ = true;
				}
			}
			if (flushing_) {
				winmm_unprepare_buffer(*buffer);
			}
		} 
	}
	
	winmm_close_device();
	stop(); // Not necessarily needed, but it seems cleaner.
	return error_type_t::ok;
}


void WinMMSink::do_set_buffers(size_t count, size_t size)
{
	buffer_count_ = count;
	buffer_size_ = size;
	init_buffers();
}

namespace {
std::map<MMRESULT, std::string> mmerror_strings = InitMap<MMRESULT, std::string>
(MMSYSERR_INVALHANDLE, "Specified device handle is invalid")
(MMSYSERR_NODRIVER, "No device driver is present")
(MMSYSERR_NOMEM, "Unable to allocate or lock memory")
(WAVERR_UNPREPARED, "The buffer pointed to by the pwh parameter hasn't been prepared")
(MMSYSERR_NOTSUPPORTED,"Function isn't supported");

std::string mmresult_to_string(MMRESULT error)
{
	auto it = mmerror_strings.find(error);
	if (it==mmerror_strings.end()) return "Unknown error";
	return it->second;
}
}
bool WinMMSink::check_call(MMRESULT res, std::string message)
{
	if (res != MMSYSERR_NOERROR) {
		logger[log_level::fatal] << message << ": " << mmresult_to_string(res);
		return false;
	}
	return true;
}
void WinMMSink::throw_call(MMRESULT res, std::string message)
{
	if (!check_call(res,message)) throw std::runtime_error(message+" ("+mmresult_to_string(res)+")");
}
void WinMMSink::throw_call(bool res, std::string message)
{
	if (!res) {
		logger[log_level::fatal] << message;
		throw std::runtime_error(message);
	}
}
}


