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
#include "iimavlib/WaveSink.h"
#include "iimavlib/AudioFilter.h"
#include "iimavlib_high_api.h"
#include "iimavlib/video_ops.h"
#ifdef SYSTEM_LINUX
#include <unistd.h>
#endif
#include <algorithm>



namespace frequency {
using iimavlib::rgb_t;
using iimavlib::rectangle_t;
using iimavlib::AudioFilter;
using iimavlib::pAudioFilter;
using iimavlib::error_type_t;
//using iimavlib::audio_buffer_t;
using iimavlib::logger;
using iimavlib::log_level;

// Value of 2*PI
const double pi2 = 8.0 * std::atan(1.0);

class Generator: public AudioFilter
{
public:
	Generator(double frequency):AudioFilter(pAudioFilter()),
	frequency_(frequency),time_(0.0),amplitude_(32767)
{
}
	void set_frequency(double frequency)
	{
		std::unique_lock<std::mutex> lock(frequency_mutex_);
		// Adjusting time so the function is continuous (this should make the sound nicer)
		if (frequency > 0.1) time_ = frequency_ * time_ / frequency;
		frequency_ = frequency;
	}
private:
	error_type_t do_process(iimavlib::audio_buffer_t& buffer)
	{
		// Prepare few values to save typing (and enable some optimizations)
		const double step = 1.0 / convert_rate_to_int(buffer.params.rate);
		std::unique_lock<std::mutex> lock(frequency_mutex_);
		for (auto& sample: buffer.data) {
			sample = amplitude_ * std::sin(time_ * frequency_ * pi2);
			time_=time_ + step;
		}
		buffer.valid_samples = buffer.data.size();
		return error_type_t::ok;
	}

	double frequency_;
	double time_;
	int16_t amplitude_;
	std::mutex frequency_mutex_;
};


namespace {
// mapping of keys to notes (Notes C4 - C6)
std::map<char, double> notes =iimavlib::InitMap<char,double>
('a', 261.63) // C4
('s', 293.66) // D4
('d', 329.63) // E4
('f', 349.23) // F4
('g', 392.00) // G4
('h', 440.00) // A4
('j', 493.88) // B4
('k', 523.25) // C5
('l', 587.33) // D5
('z', 659.25) // E5
('x', 698.46) // F5
('c', 783.99) // G5
('v', 880.00) // A5
('b', 987.77) // B5
('n', 1046.50) // C6
('m', 1174.66); //D6
}



/**
 * Example class behaving like a generator (AudioFilter) with a SDLWindow
 */
class Control: public iimavlib::SDLDevice, public AudioFilter
{
public:
	static const rgb_t black;

	Control(const pAudioFilter& child, int width, int height):
		SDLDevice(width,height,"Frequency control",true),
		AudioFilter(child),
	data_(rectangle_t(0,0,width,height),black),position_(0)
	{
		// Draw the spectrum
		rgb_t color (0,0,255);
		for (int x=0;x<width;++x) {
			double pos = static_cast<double>(x)/width;
			color.r = pos * 255;
			color.g = (1.0 - pos) * 255;
			iimavlib::draw_line(data_,rectangle_t(x,0), rectangle_t(x,height/2), color);
		}
		blit(data_);

		// Start the rendering thread
		start();
	}
	~Control() {
		// Stop the rendering thread
		stop();
	}
private:
	/// Video data
	iimavlib::video_buffer_t data_;
	/// position in the spectrum
	size_t position_;
	/// Mutex to lock @em index_ and @em position_
	std::mutex position_mutex_;

	void set_frequency(double freq) {
		auto generator = std::dynamic_pointer_cast<Generator>(get_child(0));
		generator->set_frequency(freq);
		position_=freq/2;
	}

	/**
	 * Overloaded method for handling keys from SDL window
	 * @param key  Number of the key pressed, defined in keys.h
	 * @param pressed True if the key was pressed, false if the key was released
	 * @return false if the program should end, true otherwise
	 */
	bool do_key_pressed(const int key, bool pressed) {
		using namespace iimavlib::keys;
		if (pressed) {
			auto note = notes.find(key);
			if (note != notes.end()) {
				set_frequency(note->second);
			} else {
				switch (key) {
					// If key Q or ESCAPE was pressed, we want to exit the program
					case 'q':
					case key_escape: return false;
				}
			}
		}
		update_screen(); // Update the screen　immediately to reflect the keypress
		return true;
	}

	/**
	 * Overloaded method for processing mouse buttons.
	 * @param button Index of button that triggered the event
	 * @param pressed true if button was pressed, false if released
	 * @return true, unless some fatal error has occurred
	 */
	virtual bool do_mouse_button(const int button, const bool pressed, const int x, const int /* y */)
	{
		if (button == 0 && pressed) {
			{
				std::unique_lock<std::mutex> lock(position_mutex_); // Lock the variables
				position_ = x;
			}
			auto generator = std::dynamic_pointer_cast<Generator>(get_child(0));
			generator->set_frequency(position_*2);
			update_screen();
		}
		return true;
	}
	/**
	 * Fills screen with a solid color based on the active drum's index,
	 * or with black when no there's no active drum.
	 */
	void update_screen()
	{
		// Clear the lower half of the canvas
		iimavlib::draw_rectangle(data_, rectangle_t(0,data_.size.height/2,data_.size.width,data_.size.height), black);

		{
			std::unique_lock<std::mutex> lock(position_mutex_); // Lock the variables
			iimavlib::draw_line(data_,rectangle_t(position_,data_.size.height/2), rectangle_t(position_,data_.size.height), rgb_t(255,255,0));
		}

		// And push it to the rendering thread
		blit(data_);
	}

	error_type_t do_process(iimavlib::audio_buffer_t& /*buffer*/)
	{
		if (is_stopped()) return error_type_t::failed;
		// Not touching the data, simply passing it through
		return error_type_t::ok;
	}
};

const rgb_t Control::black (0, 0, 0);
}


int main(int argc, char** argv)
try
{
	iimavlib::audio_id_t device_id = iimavlib::PlatformDevice::default_device();
	iimavlib::audio_params_t params;
	params.rate = iimavlib::sampling_rate_t::rate_44kHz;
	if (argc > 1) {
		device_id = iimavlib::simple_cast<iimavlib::audio_id_t>(argv[1]);
	}

	auto sink = iimavlib::filter_chain<frequency::Generator>(440.0)
			.add<frequency::Control>(800,600)
			.add<iimavlib::WaveSink>(std::string("aa.wav"))
			.add<iimavlib::PlatformSink>(device_id)
			.sink();

	sink->run();
}
catch (std::exception& e) {
	using namespace iimavlib;
	logger[log_level::fatal] << "The application ended unexpectedly with an error: " << e.what();
}










