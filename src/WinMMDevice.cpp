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

}

}


WinMMDevice::WinMMDevice(action_type_t action, audio_id_t id, const audio_params_t& params):
GenericDevice(),action_(action),id_(id),params_(params)
{
	sampling_rate_ 		= convert_rate_to_int(params_.rate);
	bps_ 				= get_sample_size(params_.format);

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
					0L, 						// Callback
					0L,							// Parameter to callback
					WAVE_FORMAT_DIRECT|CALLBACK_NULL),	// Mode
			"Failed to open input device");
	for(auto& hdr: buffers) init_buffer(hdr);

}
void WinMMDevice::init_buffer(WAVEHDR& hdr)
{
	hdr.lpData = (LPSTR)in_handle;
	hdr.dwBufferLength = buffer_length;
	hdr.dwBytesRecorded=0;
	hdr.dwUser = 0L;
	hdr.dwFlags = 0L;
	hdr.dwLoops = 0L;
	waveInPrepareHeader(in_handle, &hdr, sizeof(WAVEHDR));

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
}
