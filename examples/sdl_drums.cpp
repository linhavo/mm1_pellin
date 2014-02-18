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
/**
 * Example class behaving like a generator (AudioFilter) with a SDLWindow
 */
class Drums: public SDLDevice, public AudioFilter
{
public:
	static const RGB black;

	Drums(size_t width, size_t height):
		SDLDevice(width,height,"Drums"),
		AudioFilter(pAudioFilter()),
	data_(width*height,black),index_(-1),position_(0)
	{
		// Load the smaples
		load_file("../data/drum0.wav");
		load_file("../data/drum1.wav");
		load_file("../data/drum2.wav");
		logger[log_level::info] << "Drums: " << drums_.size();
		if (drums_.size()==0) throw std::runtime_error("Failed to load drum samples!");
		// Start the rendering thread
		start();
	}
	~Drums() {
		// Stop the rendering thread
		stop();
	}
private:
	/// Vector of vector to hold audio samples for the drums
	std::vector<std::vector<audio_sample_t>> drums_;
	/// Video data
	data_type data_;
	/// Index of currently playing drum
	int index_;
	/// Next sample to be played for current drum
	size_t position_;
	/// Mutex to lock @em index_ and @em position_
	std::mutex position_mutex_;

	/**
	 * Overloaded method for handling keys from SDL window
	 * @param key  Number of the key pressed, defined in keys.h
	 * @param pressed True if the key was pressed, false if the key was released
	 * @return false if the program should end, true otherwise
	 */
	bool do_key_pressed(const int key, bool pressed) {
		if (pressed) {
			switch (key) {
				// If key Q or ESCAPE was pressed, we want to exit the program
				case 'q':
				case keys::key_escape: return false;
				// Keys A, B and C should trigger drums 0, 1 and 2
				case 'a':
				case 'b':
				case 'c':
					{
						int idx = key - 'a'; // Index of current key (0 for a, 1 for b, 2 for c)
						logger[log_level::info] << "Drum "<<idx;
						std::unique_lock<std::mutex> lock(position_mutex_); // Lock the variables
						index_ 		= idx;
						position_	= 0;
					} break;
			}
		}
		update_screen(); // Update the screen　iimediately to reflect the keypress
		return true;
	}

	/**
	 * Overloaded method for processing mouse buttons.
	 * @param button Index of button that triggered the event
	 * @param pressed true if button was pressed, false if released
	 * @return true, unles some fatal error has occured
	 */
	virtual bool do_mouse_button(const int button, const bool pressed, const int, const int)
	{
		if (pressed && static_cast<size_t>(button) < drums_.size()) {
			std::unique_lock<std::mutex> lock(position_mutex_); // Lock the variables
			index_ = button;
			position_ = 0;
			logger[log_level::info] << "Playing " << index_ ;
		}
		return true;
	}
	/**
	 * Fills screen with a solid color based on the active drum's index,
	 * or with black when no there's no active drum.
	 */
	void update_screen()
	{
		/// Color to fill the screen with, default to black
		RGB color = black;
		/// Intensity of the color (255 at the beginning of the sample and gets darker as the sample continues.)
		uint8_t intensity = 0;
		{
			std::unique_lock<std::mutex> lock(position_mutex_); // Lock the variables
			if (index_ >= 0 && static_cast<size_t>(index_) < drums_.size()) {
				// Calculate the intensity for valid index
				intensity = static_cast<uint8_t>(255.0 - (2.0*255.0*position_/drums_[index_].size()));
			}
		}
		// Set the color based on sample index
		switch (index_) {
			case 0: color.r = intensity; break; // Red
			case 1: color.g = intensity; break; // Green
			case 2: color.b = intensity; break; // Blue
			default:break;
		}
		// Fill the color to out buffer
		std::fill(data_.begin(), data_.end(), color);
		// And push it to the rendering thread
		update(data_);
	}

	error_type_t do_process(audio_buffer_t& buffer)
	{
		if (is_stopped()) return error_type_t::failed;
		const audio_params_t& params = buffer.params;
//		const size_t num_channels = params.num_channels;

		// Currently only 16bit signed samples are supported
//		if (buffer.params.format != sampling_format_t::format_16bit_signed ||
//			num_channels != 2) {
//			return error_type_t::unsupported;
//		}

		// Get pointer to the raw data in the buffer (as a int16_t*)
		auto data = buffer.data.begin();

		if (index_ < 0 || (drums_.size()<=static_cast<size_t>(index_))) {
			// If there's no active drum, we just fill the buffer with zeroes
			std::fill(data,data+buffer.valid_samples,0);
		} else {
			logger[log_level::info] << "Using " << index_ << " from " << position_;
			std::unique_lock<std::mutex> lock(position_mutex_);
			// Get ref. to the current drum's buffer
			const auto& drum = drums_[index_];
			size_t samples = drums_[index_].size();
			size_t remaining = buffer.valid_samples;
			size_t written = 0;
			if (position_<samples) {
				// We still have some non-copied samples
				const size_t avail = samples - position_; // How many samples are available current drum
				written = (avail>=remaining)?remaining:avail; // We will copy this count of samples.
				auto first = drum.cbegin()+position_;		// Iterator to first sample to copy
				auto last = (avail>=remaining)?first+remaining:drum.cend(); // Iterator after the last sample that will be written
				std::copy(first,last,data); // Copy the samples to the buffer
				position_+=written; // Advance the drum's buffer position
				remaining-=written;
			} else {
				// We've already copied all the sample, so let's set current drum to none
				index_ = -1;
				position_ = 0;
			}
			// Fill the rest of the buffer (if there's still some space) with zeroes
			std::fill(data+written, data+written+remaining, 0);
		}
		// Update display (because we changed the position_)
		update_screen();
		return error_type_t::ok;
	}

	/**
	 * Loads a wave file into @em drums_ vector.
	 * @param filename Path to the file to load.
	 * @return true if the file was loaded successfully, false otherwise
	 */
	bool load_file(const std::string filename)
	{
		try {
			WaveFile wav(filename);  // Load wave file
			const audio_params_t params = wav.get_params();
			if (params.rate != sampling_rate_t::rate_44kHz) throw std::runtime_error("Wrong sampling rate. 44kHz expected.");
//			if (params.format != sampling_format_t::format_16bit_signed) throw std::runtime_error("Wrong sampling format. Signed 16bits expected.");
//			if (params.num_channels != 2) throw std::runtime_error("Wrong number of channel. Expected stereo file.");
			size_t samples = 44100;
			std::vector<audio_sample_t> data(samples);
			wav.read_data(data,samples);
			data.resize(samples);
			logger[log_level::info] << "Read " << samples << "samples";
			drums_.push_back(std::move(data));
		}
		catch (std::exception &e) {
			logger[log_level::fatal] << "Failed to load " << filename << " (" << e.what() << ")";
			return false;
		}
		return true;
	}
};
const RGB Drums::black = {0, 0, 0};
}


int main()
try
{
	iimavlib::audio_id_t device_id = iimavlib::PlatformDevice::default_device();
	iimavlib::audio_params_t params;
	params.rate = iimavlib::sampling_rate_t::rate_44kHz;
	auto sink = iimavlib::filter_chain<iimavlib::Drums>(800,600)
			.add<iimavlib::PlatformSink>(device_id)
			.sink();

//	sink->set_buffers(1,128);
	sink->run();
}
catch (std::exception& e) {
	using namespace iimavlib;
	logger[log_level::fatal] << "The application ended unexpectedly with an error: " << e.what();
}










