/*
 * WinMMDevice.cpp
 *
 *  Created on: 20.1.2013
 *      Author: neneko
 */
#include "iimaudio/WinMMDevice.h"
#include "iimaudio/Utils.h"
#include <map>
#include <stdexcept>
#include <algorithm>

#pragma comment(lib, "winmm.lib")
namespace iimaudio {

namespace {
void CALLBACK win_mm_device_capture_callback (HWAVEIN hwi, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
	//logger[log_level::info] << "CALLBACK, msg: " << uMsg << "\n";
	if (uMsg == WIM_DATA) {
		WinMMDevice *winmm = reinterpret_cast<WinMMDevice*>(dwInstance);
		WAVEHDR *hdr = reinterpret_cast<WAVEHDR*>(dwParam1);
		winmm->store_data(*hdr);
	}
}

}


WinMMDevice::WinMMDevice(action_type_t action, audio_id_t id, const audio_params_t& params):
	GenericDevice(),action_(action),id_(id),params_(params),private_buffer_(1048576)
{
	sampling_rate_ 		= convert_rate_to_int(params_.rate);
	bps_ 				= get_sample_size(params_.format) * 8;

	WAVEFORMATEX fmt;
	fmt.wFormatTag		= WAVE_FORMAT_PCM;
	fmt.nChannels		= params_.num_channels;
	fmt.nSamplesPerSec	= sampling_rate_;
	fmt.nAvgBytesPerSec	= sampling_rate_*params_.num_channels*bps_/8;   // = nSamplesPerSec * n.Channels * wBitsPerSample/8
	fmt.nBlockAlign		= params_.num_channels*bps_/8;                  // = n.Channels * wBitsPerSample/8
	fmt.wBitsPerSample	= bps_;
	fmt.cbSize			= 0;

	buffers.resize(buffer_count);
	switch (action) {
		case action_type_t::action_capture:
			init_capture(fmt);
			break;
		default:
			throw_call(false, "Unsupported action!");
	}

}

void WinMMDevice::init_capture(WAVEFORMATEX& fmt)
{
	throw_call(waveInOpen(&in_handle,			// Input handle
					id_ ,						// Device ID
					&fmt,						// Input format
					reinterpret_cast<DWORD_PTR>(&win_mm_device_capture_callback), 
												// Callback
					reinterpret_cast<DWORD_PTR>(this),
												// Parameter to callback
					WAVE_FORMAT_DIRECT|CALLBACK_FUNCTION),	// Mode
			"Failed to open input device");
	//private_buffer_.data.resize(buffer_count*buffer_length*2);

	for(auto& hdr: buffers) init_buffer(hdr);
	logger[log_level::debug] << "Opened device and added " << buffers.size() << " buffers\n";


}
void WinMMDevice::init_buffer(WAVEHDR& hdr)
{
	hdr.lpData = (LPSTR)new uint8_t[buffer_length];
	hdr.dwBufferLength = buffer_length;
	hdr.dwBytesRecorded=0;
	hdr.dwUser = 0L;
	hdr.dwFlags = 0L;
	hdr.dwLoops = 0L;
	throw_call(waveInPrepareHeader(in_handle, &hdr, sizeof(WAVEHDR)),"Failed to prepare buffer");
	throw_call(waveInAddBuffer(in_handle, &hdr, sizeof(WAVEHDR)),"Failed to add buffer");
}
WinMMDevice::~WinMMDevice()
{
}

WinMMDevice::audio_id_t WinMMDevice::default_device()
{
	return WAVE_MAPPER;
}

namespace {
std::map<MMRESULT, std::string> mmerror_strings = InitMap<MMRESULT, std::string>
(MMSYSERR_INVALHANDLE, "Specified device handle is invalid")
(MMSYSERR_NODRIVER, "No device driver is present")
(MMSYSERR_NOMEM, "Unable to allocate or lock memory")
(WAVERR_UNPREPARED, "The buffer pointed to by the pwh parameter hasn't been prepared");

std::string mmresult_to_string(MMRESULT error)
{
	auto it = mmerror_strings.find(error);
	if (it==mmerror_strings.end()) return "Unknown error";
	return it->second;
}
}
bool WinMMDevice::check_call(MMRESULT res, std::string message)
{
	if (res != MMSYSERR_NOERROR) {
		logger[log_level::fatal] << message << ": " << mmresult_to_string(res)<< "\n";
		return false;
	}
	return true;
}
void WinMMDevice::throw_call(MMRESULT res, std::string message)
{
	if (!check_call(res,message)) throw std::runtime_error(message+" ("+mmresult_to_string(res)+")");
}
void WinMMDevice::throw_call(bool res, std::string message)
{
	if (!res) {
		logger[log_level::fatal] << message << std::endl;
		throw std::runtime_error(message);
	}
}
return_type_t WinMMDevice::do_start_capture() {
	if(!check_call(waveInStart(in_handle),"Failed to start capture")) return return_type_t::failed;
	return return_type_t::ok;
}

size_t WinMMDevice::do_capture_data(uint8_t* data_start, size_t data_size, return_type_t& error_code) 
{
	std::lock_guard<std::mutex> l(buffer_lock_);
	while (!empty_buffers.empty()) {
		WAVEHDR& hdr = *empty_buffers.back();
		check_call(waveInUnprepareHeader(in_handle, &hdr, sizeof(WAVEHDR)),"Failed to unprepare buffer");
		private_buffer_.store_data(reinterpret_cast<uint8_t*>(hdr.lpData),hdr.dwBytesRecorded);
		logger[log_level::debug] << "Stored "<< hdr.dwBytesRecorded << " bytes into circular buffer\n";
		check_call(waveInPrepareHeader(in_handle, &hdr, sizeof(WAVEHDR)),"Failed to prepare buffer");
		check_call(waveInAddBuffer(in_handle, &hdr, sizeof(WAVEHDR)),"Failed to add buffer");
		empty_buffers.pop_back();
	}

	std::size_t ret = private_buffer_.get_data_block(data_start,data_size);
	if (ret == 0) error_code = return_type_t::buffer_empty;
	else error_code = return_type_t::ok;
	return ret/params_.sample_size();
}
void WinMMDevice::store_data(WAVEHDR& hdr)
{
	std::lock_guard<std::mutex> l(buffer_lock_);
	empty_buffers.push_back(&hdr);
}

audio_info_t get_info(UINT dev)
{
	audio_info_t info_;
	WAVEINCAPS caps_;
	waveInGetDevCaps (dev,&caps_,sizeof(WAVEINCAPS));
	info_.max_channels = caps_.wChannels;
	info_.name = caps_.szPname;
	info_.default = false;
	return info_;
}
std::map<WinMMDevice::audio_id_t, audio_info_t> WinMMDevice::do_enumerate_capture_devices() 
{
	std::map<audio_id_t, audio_info_t> devices;
	devices[WAVE_MAPPER]=get_info(WAVE_MAPPER);
	UINT num_dev = waveInGetNumDevs();
	for (UINT i=0;i<num_dev;++i) devices[i]=get_info(i);
	
	devices[default_device()].default=true;
	return devices;
}
}