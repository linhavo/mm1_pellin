/**
 * @file 	AlsaDevice.cpp
 *
 * @date 	19.1.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 */

#include "iimaudio/AlsaDevice.h"
#include "iimaudio/Utils.h"
#include <stdexcept>
#include <iostream>
#include <algorithm>

namespace iimaudio {

namespace {
std::map<sampling_format_t, snd_pcm_format_t> format_mapping = InitMap<sampling_format_t, snd_pcm_format_t>
(sampling_format_t::format_8bit_unsigned, 	SND_PCM_FORMAT_U8)
(sampling_format_t::format_8bit_signed, 	SND_PCM_FORMAT_S8)
(sampling_format_t::format_16bit_unsigned, 	SND_PCM_FORMAT_U16)
(sampling_format_t::format_16bit_signed, 	SND_PCM_FORMAT_S16);

snd_pcm_format_t convert_format_to_alsa(sampling_format_t format)
{
	auto it = format_mapping.find(format);
	if (it == format_mapping.end()) return SND_PCM_FORMAT_UNKNOWN;
	return it->second;
}
}

AlsaDevice::AlsaDevice(action_type_t action, audio_id_t id, const audio_params_t& params)
:GenericDevice(),action_(action),id_(id),params_(params),handle_(nullptr),sample_size_(0),
 first_empty_buffer(0),first_full_buffer(0)
{

	switch (action) {
		case action_type_t::action_capture: stream_type_ = SND_PCM_STREAM_CAPTURE;
				break;
		case action_type_t::action_playback: stream_type_ = SND_PCM_STREAM_PLAYBACK;
				break;
		default:
			throw_call(false,"Wrong action type specified");
	}

	sampling_rate_ = convert_rate_to_int(params_.rate);
	throw_call(sampling_rate_ > 0, "Unsupported sampling rate");

	snd_pcm_format_t alsa_fmt = convert_format_to_alsa(params_.format);
	throw_call(alsa_fmt != SND_PCM_FORMAT_UNKNOWN, "Unsupported sampling format");
	snd_pcm_hw_params_t *hw_params;
	snd_pcm_sw_params_t *sw_params;

	throw_call(snd_pcm_open (&handle_, id.c_str(), stream_type_, 0),
			"Failed to open device for capture");

	logger[log_level::debug] << "Device '" << id << "' opened" << std::endl;



	// Initialize HW params struct
	throw_call(snd_pcm_hw_params_malloc (&hw_params),
				"Failed to allocate HW params");
	throw_call(snd_pcm_hw_params_any (handle_, hw_params),
			"Failed to initialize HW params");

	// Set access type to interleaved
	throw_call(snd_pcm_hw_params_set_access (handle_, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED),
			"Failed to set access type");

	throw_call(snd_pcm_hw_params_set_format (handle_, hw_params, alsa_fmt),
				"Failed to set format");

	int dir = 0;

	throw_call(snd_pcm_hw_params_set_rate_near (handle_, hw_params, &sampling_rate_, &dir),
			"Failed to set sample rate");
	logger[log_level::info] << "Initialized for " << sampling_rate_ << " Hz" << std::endl;

	throw_call(snd_pcm_hw_params_set_channels (handle_, hw_params, params_.num_channels),
				"Failed to set number of channels");
	logger[log_level::info] << "Initialized for " << static_cast<int>(params_.num_channels) << " channels" << std::endl;

	throw_call(snd_pcm_hw_params (handle_, hw_params),
				"Failed to set params");

	snd_pcm_hw_params_free (hw_params);

	if (stream_type_ == SND_PCM_STREAM_PLAYBACK) {
		throw_call(snd_pcm_sw_params_malloc (&sw_params),
				"cannot allocate software parameters structure");
		throw_call(snd_pcm_sw_params_current (handle_, sw_params),
				"cannot initialize software parameters structure");
		throw_call(snd_pcm_sw_params_set_avail_min (handle_, sw_params, 4096)
				,"cannot set minimum available count");
		throw_call(snd_pcm_sw_params_set_start_threshold (handle_, sw_params, 0U),
					"cannot set start mode");
		throw_call(snd_pcm_sw_params (handle_, sw_params),
					"cannot set software parameters");

		snd_pcm_sw_params_free (sw_params);
	}

	sample_size_ = params_.num_channels * get_sample_size(params_.format);
	check_call(sample_size_ > 0, "Wrong sample format");
	logger[log_level::info] << "Device '" << id << "' initialized" << std::endl;


}

AlsaDevice::~AlsaDevice()
{
	check_call(snd_pcm_close (handle_),
			"Failed to close the device");

	logger[log_level::debug] << "Device '" << id_ << "' closed" << std::endl;
}

AlsaDevice::audio_id_t AlsaDevice::default_device()
{
	return std::string("default");
}

bool AlsaDevice::check_call(int res, std::string message)
{
	if (res < 0) {
		logger[log_level::fatal] << message << ": " << snd_strerror(res)<< std::endl;
		return false;
	}
	return true;
}
void AlsaDevice::throw_call(int res, std::string message)
{
	if (!check_call(res,message)) throw std::runtime_error(message+" ("+std::string(snd_strerror(res))+")");
}
void AlsaDevice::throw_call(bool res, std::string message)
{
	if (!res) {
		logger[log_level::fatal] << message << std::endl;
		throw std::runtime_error(message);
	}
}

return_type_t AlsaDevice::do_start_capture()
{
	if (!check_call(snd_pcm_prepare (handle_), "Failed to prepare PCM"))
		return return_type_t::failed;
	return return_type_t::ok;
}
return_type_t AlsaDevice::do_start_playback()
{
	return do_start_capture();
}
return_type_t AlsaDevice::do_set_buffers(uint16_t count, uint32_t samples)
{
	buffers.resize(count);
	const uint32_t size = samples * params_.sample_size();
	logger[log_level::debug] << "Allocating " << count << " buffers of size " << size << " Bytes (" << samples << " samples)\n";
	std::for_each(buffers.begin(), buffers.end(),
			[size](audio_buffer_t& buf){buf.data.resize(size);buf.empty=true;buf.position=0;});
	return return_type_t::ok;
}

return_type_t AlsaDevice::do_update(size_t delay)
{
	audio_buffer_t &buf = buffers[first_full_buffer];
	if (buf.empty) return return_type_t::buffer_empty;

	int ret = snd_pcm_wait(handle_, delay);
	if (!ret) return return_type_t::busy;
	size_t frames_free = snd_pcm_avail(handle_);
	if (!frames_free) return return_type_t::invalid;

	size_t write_frames = std::min(frames_free,(buf.data.size()-buf.position)/params_.sample_size());
	write_frames = snd_pcm_writei(handle_,reinterpret_cast<void*>(&buf.data[buf.position]),write_frames);
	buf.position+=write_frames*params_.sample_size();
	if (buf.position>=buf.data.size()) {
		first_full_buffer = (first_full_buffer+1)%buffers.size();
		buf.position = 0;
		buf.empty = true;
	}
	return return_type_t::ok;
}
audio_params_t AlsaDevice::do_get_params() const
{
	return params_;
}

return_type_t AlsaDevice::do_fill_buffer(const uint8_t* data_start, size_t data_size)
{
	if (first_empty_buffer >= buffers.size()) return return_type_t::invalid;
	audio_buffer_t &buf = buffers[first_empty_buffer];
	if (!buf.empty) return return_type_t::buffer_full;
	size_t copy_bytes = std::min(buf.data.size(),data_size);
	std::copy_n(data_start,copy_bytes,buf.data.begin());
	buf.position = 0;
	buf.empty = false;
	if (buffers[first_full_buffer].empty) first_full_buffer = first_empty_buffer;
	first_empty_buffer = (first_empty_buffer+1)%buffers.size();
	return return_type_t::ok;
}
size_t AlsaDevice::do_capture_data(uint8_t* data_start, size_t data_size, return_type_t& error_code)
{
	int ret;
	const unsigned long buffer_size = static_cast<unsigned long>(data_size/sample_size_);
	if (!check_call(ret = snd_pcm_readi(handle_,reinterpret_cast<void*>(data_start),
						buffer_size),
					"Failed to read data"))
	{
		if (ret == -EPIPE) {
			error_code = return_type_t::xrun;
		} else if (ret <0) error_code = return_type_t::failed;
		return 0;
	}
	error_code = return_type_t::ok;
	return static_cast<size_t>(ret);
}

std::map<AlsaDevice::audio_id_t, audio_info_t> AlsaDevice::do_enumerate_capture_devices()
{
	std::map<audio_id_t, audio_info_t> devices;
	enumerate_hw_devices(devices, SND_PCM_STREAM_CAPTURE);
//        devices[WAVE_MAPPER]=get_info(WAVE_MAPPER);
//        UINT num_dev = waveInGetNumDevs();
//        for (UINT i=0;i<num_dev;++i) devices[i]=get_info(i);

//        devices[default_device()].default_=true;
        return devices;
}

std::map<AlsaDevice::audio_id_t, audio_info_t> AlsaDevice::do_enumerate_playback_devices()
{
	std::map<audio_id_t, audio_info_t> devices;
	enumerate_hw_devices(devices, SND_PCM_STREAM_PLAYBACK);
//        devices[WAVE_MAPPER]=get_info(WAVE_MAPPER);
//        UINT num_dev = waveInGetNumDevs();
//        for (UINT i=0;i<num_dev;++i) devices[i]=get_info(i);

//        devices[default_device()].default_=true;
        return devices;
}
void AlsaDevice::enumerate_hw_devices(std::map<audio_id_t, audio_info_t>&map_, snd_pcm_stream_t type_)
{
	snd_ctl_t *handle;
	snd_ctl_card_info_t *info;
	snd_pcm_info_t *pcminfo;
	snd_ctl_card_info_alloca(&info);
	snd_pcm_info_alloca(&pcminfo);

	int id = -1;
	int err;
	int dev = -1;
	while (snd_card_next(&id)==0) {
		if (id<0) break;
		std::stringstream ss;
		ss << "hw:" << id;
//		logger[log_level::info] << ss.str() << "\n";
		err = snd_ctl_open(&handle, ss.str().c_str(), 0);
		if (err<0) continue;
		err = snd_ctl_card_info(handle, info);
		if (err<0) {
			snd_ctl_close(handle);
			continue;
		}
		dev = -1;
		while (snd_ctl_pcm_next_device(handle, &dev)==0) {
			if (dev<0) break;
//			logger[log_level::info] << dev << "\n";
			snd_pcm_info_set_device(pcminfo, dev);
			snd_pcm_info_set_subdevice(pcminfo, 0);
			snd_pcm_info_set_stream(pcminfo, type_);
			err = snd_ctl_pcm_info(handle, pcminfo);
			if (err<0) {
//				logger[log_level::info] <<  "err: " << snd_strerror(err) << "\n";
				continue;
			}
//			logger[log_level::info] <<  "XX\n";
			std::stringstream ss2;
			ss2 << "hw:" << id <<","<<dev;
			std::string hw = ss2.str();
			std::stringstream ss3;
			ss3 << snd_ctl_card_info_get_id(info) << " [" << snd_ctl_card_info_get_name(info) << "] "
					<< snd_pcm_info_get_id(pcminfo) << " [" << snd_pcm_info_get_name(pcminfo)<<"]";
			audio_info_t info_;
			info_.name = ss3.str();
			map_[hw]=info_;

		}
		snd_ctl_close(handle);
	}
//	snd_ctl_card_info_free(info);
//	snd_pcm_info_free(pcminfo);
}


}


