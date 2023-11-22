/**
 * @file 	sdl_sequencer.cpp
 *
 * @date 	23.10.2023
 * @author 	Ondrje Slaby <slaby@iim.cz>
 * @copyright GNU Public License 3.0
 *
 */
#include "iimavlib/AudioFFT.h"
#include "iimavlib/AudioTypes.h"
#include "SDL/SDL_video.h"
#include "iimavlib/WaveSource.h"
#include "iimavlib/filters/SineMultiply.h"


#include <iimavlib/SDLDevice.h>
#include <iimavlib/AudioFilter.h>
#include <iimavlib_high_api.h>
#include <iimavlib/video_ops.h>
#include <iimavlib/Utils.h>
#include <iimavlib/keys.h>
#include <mutex>
#include <cmath>

#include <algorithm>
#include <atomic>
#include <functional>
#include "../src/video_ops.cpp"


using namespace iimavlib;


namespace {
// Max value for int16_t
const double max_val = std::numeric_limits<int16_t>::max();

// Value of 2*PI
const double pi2 = 8.0 * std::atan(1.0);
}

/**
 * An interface to enable or disable a filter in a thread-safe manner.
 * Uses a mutex to facilitate an atomic toggle. Calls the virtual method
 * reinitialize when the filter is enabled again to allow resetting time or
 * any other variable.
 */
class ToggleableFilter
{
public:
	void set_enabled(bool enabled)
	{
		std::unique_lock<std::mutex> lock(enabled_mutex_);
		enabled_ = enabled;
		if (enabled)
			reinitialize();
	}

	inline bool is_enabled()
	{
		std::unique_lock<std::mutex> lock(enabled_mutex_);
		return enabled_;
	}
private:
	virtual void reinitialize() = 0;

	bool enabled_ = false;
	std::mutex enabled_mutex_;
};

/**
 * Simple sine wave generator. See the playback_sine.cpp example for details.
 * This class additionally implements disabling, which means we have to clear
 * the sample buffer when not generating, otherwise it will contain old/garbage data.
 */
class SineGenerator : public AudioFilter, public ToggleableFilter
{
public:
	SineGenerator(float frequency) : AudioFilter(pAudioFilter()), ToggleableFilter(), frequency_(frequency), time_(0.0f)
	{

	}

	error_type_t do_process(audio_buffer_t& buffer) override
	{
		// If disabled, clear the output buffer
		if (!is_enabled())
		{
			for (auto& sample : buffer.data)
			{
				sample = 0;
			}
			return error_type_t::ok;
		}

		const double step = 1.0 / convert_rate_to_int(buffer.params.rate);

		for (auto& sample : buffer.data) {
			sample = static_cast<int16_t>(max_val * std::sin(time_ * frequency_ * pi2));
			time_ = time_ + step;
		}
		buffer.valid_samples = buffer.data.size();
		return error_type_t::ok;
	}

private:
	double frequency_;
	double time_;

	void reinitialize() override
	{
		time_ = 0.0f;
	}
};

/**
 * Since there can only be one true generator and everything else processes its signal,
 * this class adds the generated square wave (if it is enabled) to the already generated
 * output of the sine generator. This means it cannot be first in the filter chain.
 * It could be generalized to allow having it first by implementing another constructor
 * (same as the sine generator) and saving whether it is the first generator.
 */
class SquareAdder : public AudioFilter, public ToggleableFilter
{
public:
	SquareAdder(const pAudioFilter& child, float frequency) : AudioFilter(child), frequency_(frequency), time_(0.0f)
	{

	}

	error_type_t do_process(audio_buffer_t& buffer) override
	{
		// If disabled, just skip processing and leave existing data alone
		if (!is_enabled()) return error_type_t::ok;

		const double step = 1.0 / convert_rate_to_int(buffer.params.rate);

		for (auto& sample : buffer.data) {
			// Insead of overwriting sample data, add out own generated signal to it, so the original
			// signal doesn't get lost.
			sample += static_cast<int16_t>(std::copysignl(max_val, std::sin(time_ * frequency_ * pi2)));
			time_ = time_ + step;
		}
		buffer.valid_samples = buffer.data.size();
		return error_type_t::ok;
	}

private:
	double frequency_;
	double time_;

	void reinitialize() override
	{
		time_ = 0.0f;
	}
};

/**
 * Example class facilitating the control of two separate signal generators (appended above this controller in the chain).
 * The controller accesses its instruments by going up the filter chain. It expects the specified number of generators set
 * in the constructor to implement the ToggleableFilter interface. It then uses these interfaces to enable/disable generators
 * as per user input on the current timeline.
 */
class Control : public SDLDevice, public AudioFilter
{
public:
	Control(const pAudioFilter& child, int width, int height, int instruments, int steps, float loop_length) : SDLDevice(width, height, "Sequencer"), 
		AudioFilter(child), instruments_(instruments), steps_(steps), last_step_(-1), loop_length_(loop_length), time_(0.0f), data_(width, height)
	{
		timespec_ = 10.0f/1000.0f;
		sequence_.resize(instruments * steps, false);
		
		const audio_params_t& params = get_params();
		cache_size_ = static_cast<size_t>(timespec_ * convert_rate_to_int(params.rate));
		cache_size_ = static_cast<size_t>(pow(2, ceil(log2(cache_size_))) * 2);
		logger[log_level::info] << "Cache size: " << cache_size_;
		sample_cache_.resize(cache_size_);
		barwidth = 40;
		x_count = 0;
		height_ = height/2;
		width_ = width;

		thread_ = std::thread(std::bind(&Control::execute_thread, this));
		time_elapsed = 0.000f;
		const auto max_int16_value = std::numeric_limits<int16_t>::max();

		magic_constant = 1.0f / 16384.0f / max_int16_value;


		start();

	}

	~Control()
	{
		stop();
	}

private:

	double timespec_;	
	int instruments_;
	int steps_;
	int last_step_;
	double loop_length_;
	double time_;
	std::thread thread_;
	std::mutex mutex_;
	std::vector<audio_sample_t> sample_cache_;
	int width_;
	int x_count;
	int height_;
	int barwidth;
	std::atomic<bool> end_;
	std::atomic<bool> changed_;
	size_t last_sample_;
	size_t cache_size_;
	std::vector<complexarray_t<float>> coefficient_array_entire;
	float time_elapsed;
	float whole;
	float magic_constant;


	AudioFFT<float> fft;

	/// Video data
	iimavlib::video_buffer_t data_;
	/// Sequence data
	std::vector<bool> sequence_;

	void execute_thread() {
		logger[log_level::debug] << "Drawing thread started";
		while (!end_) {
			if (changed_) {
				draw_wave();
			}
			if (!blit(data_)) {
				logger[log_level::debug] << "Drawing thread finishing";
				end_ = true;
			}
		}
		auto surface = SDL_GetVideoSurface();
		SDL_SaveBMP(surface, "screenshot.bmp");
	}


	void update_cache(const audio_buffer_t& buffer) {
		std::unique_lock<std::mutex> lock(mutex_);
		const audio_sample_t* src = &buffer.data[0];
		size_t src_remaining = buffer.valid_samples;

		while (src_remaining) {
			const size_t to_copy = std::min(src_remaining, sample_cache_.size() - last_sample_);
			std::copy_n(src, to_copy, &sample_cache_[0] + last_sample_);
			last_sample_ += to_copy;
			if (last_sample_ >= sample_cache_.size()) last_sample_ = 0;
			src_remaining -= to_copy;
		}
		changed_.store(true);
	}




	void draw_wave() {
		// Max value for int16_t

		changed_.store(false);

		// Array for the coefficients from FFT
		complexarray_t<float> coefficient_array;
		{
			std::unique_lock<std::mutex> lock(mutex_);
			coefficient_array = fft.FFT1D(sample_cache_.begin(), sample_cache_.end());
		}

		const auto unique_coefficients = (coefficient_array.size() + 1) / 2;

		double loop_fraction = time_ / loop_length_;

		for (int y = 0; y < height_; ++y) {
			const size_t coefficient_number = y * unique_coefficients / height_;

			// Get value for the coefficient
			auto coefficient = std::abs(coefficient_array[coefficient_number]) * magic_constant;
			coefficient *= 100;

			auto yycol = static_cast<int>(255 * (coefficient));

			std::vector<int> colr = { 1, 0 , 0 };
			colr[0] = int(yycol);
			colr[2] = int(50 * y / height_);
			for (auto i = 0; i < 3; i++) {
				colr[i] += int(yycol ^ 2);
				if (colr[i] > 255) {
					colr[i] = 255;
				}
			}
			draw_bars(static_cast<int>(loop_fraction * data_.size.width), y, colr);
		}
		//sample_cache_.clear();
	}


	void draw_bars(int x, int y, std::vector<int> colr) {
		rectangle_t rectangle = intersection(data_.size, rectangle_t(x, y, barwidth / 20, (data_.size.height / 2) - y));
		iimavlib::draw_rectangle(data_, rectangle_t(rectangle.x, rectangle.y, rectangle.width, 3), rgb_t(colr[0], colr[1], colr[2]));
	}

	void draw_sequence()
	{
		// Set up our display colors
		const rgb_t enabled_color = {20, 200, 50};
		const rgb_t disabled_color = {20, 20, 20};

		// What's the size of each sequencer step (width) and of each instrument row (height)
		const int step_size = data_.size.width / steps_;
		const int inst_size = (data_.size.height / 2) / instruments_;

		// How far we are through the current loop
		double loop_fraction = time_ / loop_length_;

		// Draw all rectangles representing each sequencer step + instrument
		for (int i = 0; i < steps_; ++i)
		{
			for (int j = 0; j < instruments_; ++j)
			{
				const rectangle_t rect = {i * step_size, (j * inst_size) + (data_.size.height / 2), step_size, inst_size};
				draw_rectangle(data_, rect, sequence_[i * instruments_ + j] ? enabled_color : disabled_color);
			}
		}

		// Draw a moving cursor to indicate current progress through the loop
		draw_line(data_, rectangle_t(static_cast<int>(loop_fraction * data_.size.width), (data_.size.height / 2)), rectangle_t(static_cast<int>(loop_fraction * data_.size.width), (data_.size.height)), rgb_t(255, 255, 0));
		//blit(data_);
	}
	inline int current_step()
	{
		return static_cast<int>(steps_ * time_ / loop_length_);
	}

	/**
	 * Overrides the parent method. This is where we perform the actual actions of enabling/disabling generators.
	 * We also trigger a redraw here.
	 */
	error_type_t do_process(audio_buffer_t& buffer) override
	{
		if (is_stopped()) return error_type_t::failed;
		// Not touching the data, simply passing it through
		// But update graphics and control generators

		// Update our loop time (and loop it if appropriate)
		time_ += buffer.valid_samples * 1.0 / convert_rate_to_int(buffer.params.rate);
		if (time_ > loop_length_) time_ -= loop_length_;
		draw_sequence();

		update_cache(buffer);

		// Get our current step and check if we want any generators enabled
		int cur_step = current_step();
		if (cur_step != last_step_)
		{
			for (int i = 0; i < instruments_; ++i)
			{
				// If we want a generator enabled, enable it, otherwise it gets disabled
				std::shared_ptr<ToggleableFilter> filter = std::dynamic_pointer_cast<ToggleableFilter>(get_child(i));
				filter->set_enabled(sequence_[cur_step * instruments_ + i]);
			}

			// We only enable/disable at step boundary to save some processing, so we have to remember which step we just processed
			last_step_ = cur_step;
		}


		return error_type_t::ok;
	}

	/**
	 * Overrides the key press handling method. We have to handle a quit key ourselves (or pass it to the parent class).
	 * Otherwise, when a specific key is pressed, toggle a generator at the current sequencer step.
	 */
	bool do_key_pressed(const int key, bool pressed) override
	{
		if (!pressed)
			return true;
		if (keys::key_q == key)
			return false;

		int filter_index = std::max(std::min(instruments_, (key - keys::key_a)), 0);
		int cur_step = current_step();
		sequence_[cur_step * instruments_ + filter_index] = !sequence_[cur_step * instruments_ + filter_index];

		return true;
	}
};

int main(int argc, char** argv)
try
{
	iimavlib::audio_id_t device_id = iimavlib::PlatformDevice::default_device();
	iimavlib::audio_params_t params;
	params.rate = iimavlib::sampling_rate_t::rate_44kHz;
	if (argc > 1) {
		device_id = iimavlib::simple_cast<iimavlib::audio_id_t>(argv[1]);
	}

	auto sink = iimavlib::filter_chain<SineGenerator>(440.0)
		.add<SquareAdder>(780.0)
		.add<SquareAdder>(880.0)
		.add<Control>(800, 400, 3, 16, 5.0f)
		.add<iimavlib::PlatformSink>(device_id)
		.sink();

	sink->run();
}
catch (std::exception& e) {
	using namespace iimavlib;
	logger[log_level::fatal] << "The application ended unexpectedly with an error: " << e.what();
}