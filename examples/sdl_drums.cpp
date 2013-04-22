/**
 * @file 	sdl_drums.cpp
 *
 * @date 	21.4.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 */

#include "iimavlib/SDLDevice.h"
#include "iimavlib/Utils.h"
#include "iimavlib/WaveFile.h"
#include "iimavlib/AudioFilter.h"
#include "iimavlib_high_api.h"
#ifdef SYSTEM_LINUX
#include <unistd.h>
#endif
#include <algorithm>

namespace iimavlib {
class Drums: public SDLDevice, public AudioFilter
{
public:
	const RGB black = {0,0,0};
	Drums(size_t width, size_t height):
		SDLDevice(width,height,"Drums"),
		AudioFilter(pAudioFilter()),
	data_(width*height,black),index_(-1),position_(0)
	{
		load_file("../data/drum0.wav");
		load_file("../data/drum1.wav");
		load_file("../data/drum2.wav");
		logger[log_level::info] << "Drums: " << drums_.size();
		update(data_);
		start();
	}
	~Drums() {
		stop();
	}
private:
	std::vector<std::vector<int16_t>> drums_;
	data_type data_;
	int index_;
	size_t position_;
	bool do_key_pressed(const int key, bool pressed) {
		if (pressed) {
			switch (key) {
				case keys::key_escape: return false;
				case 'a':
				case 'b':
				case 'c':
					{
						int idx = key - 'a';
						logger[log_level::info] << "Drum "<<idx;
						index_ = idx;
						position_=0;
					}break;
			}
		}
		update_screen();
		return true;
	}
	void update_screen()
	{
		RGB color = black;
		switch (index_) {
			case 0: color = RGB{255, 0, 0};break;
			case 1: color = RGB{0, 255, 0};break;
			case 2: color = RGB{0, 0, 255};break;
			default:break;
		}
		std::fill(data_.begin(), data_.end(), color);
		update(data_);
	}
	error_type_t do_process(audio_buffer_t& buffer)
	{
		if (is_stopped()) return error_type_t::failed;
		const audio_params_t& params = buffer.params;
		const size_t num_channels = params.num_channels;
		const size_t rate_int =convert_rate_to_int(params.rate);

		// Currently only 16bit signed samples are supported
		if (buffer.params.format != sampling_format_t::format_16bit_signed ||
			num_channels != 2) {
			return error_type_t::unsupported;
		}


		// Get pointer to the raw data in the buffer (as a int16_t*)
		int16_t * data = reinterpret_cast<int16_t*>(&buffer.data[0]);
//		logger[log_level::debug] << "Index: " << index_ << ", drums: " << drums_.size();
		if (index_ < 0 || (drums_.size()<=index_)) {
			std::fill(data,data+buffer.valid_samples*num_channels,0);
		} else {
			const auto& drum = drums_[index_];
			size_t samples = drums_[index_].size()/2; //Divided by 2 because we have 2 samples for channels
			size_t remaining = buffer.valid_samples;
			size_t written = 0;
			if (position_<samples) {
				const size_t avail = samples - position_;
				written = (avail>=remaining)?remaining:avail;
//				logger[log_level::debug] << "Writting " << written << " samples (valid: " << remaining << ")";
				auto first = drum.cbegin()+position_*2;
				auto last = (avail>=remaining)?first+remaining*2:drum.cend();
				std::copy(first,last,data);
				position_+=written;
				remaining-=written;
			} else {
				index_ = -1;
				position_ = 0;
				update_screen();
			}
			std::fill(data+written*2, data+(written+remaining)*2, 0);
		}
		return error_type_t::ok;
	}
	bool load_file(const std::string filename)
	{
		try {
			WaveFile wav(filename);
			const audio_params_t params = wav.get_params();
			if (params.rate != sampling_rate_t::rate_44kHz) throw std::runtime_error("Wrong sampling rate. 44kHz expected.");
			if (params.format != sampling_format_t::format_16bit_signed) throw std::runtime_error("Wrong sampling format. Signed 16bits expected.");
			if (params.num_channels != 2) throw std::runtime_error("Wrong number of channel. Expected stereo file.");
			size_t samples = 44100;
			std::vector<int16_t> data(samples*2);
			wav.read_data(data,samples);
			data.resize(samples*2);
			logger[log_level::info] << "Read " << samples << "samples";
			drums_.push_back(std::move(data));
		}
		catch (std::exception &e) {
			logger[log_level::fatal] << "Failed to load " << filename;
			return false;
		}
		return true;
	}
};
}
int main()
{
	iimavlib::audio_id_t device_id = iimavlib::PlatformDevice::default_device();
	auto sink = iimavlib::filter_chain<iimavlib::Drums>(800,600)
			.add<iimavlib::PlatformSink>(device_id)
			.sink();

	sink->run();
}











