/**
 * @file 	sdl_sequencer.cpp
 *
 * @date 	23.10.2023
 * @author 	Ondrje Slaby <slaby@iim.cz>
 * @copyright GNU Public License 3.0
 *
 */

#include <iimavlib/SDLDevice.h>
#include <iimavlib/AudioFilter.h>
#include <iimavlib_high_api.h>
#include <iimavlib/video_ops.h>
#include <iimavlib/Utils.h>
#include <iimavlib/keys.h>
#include <mutex>


using namespace iimavlib;


namespace {
// Max value for int16_t
const double max_val = std::numeric_limits<int16_t>::max();

// Value of 2*PI
const double pi2 = 8.0 * std::atan(1.0);
}

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

class SineGenerator : public AudioFilter, public ToggleableFilter
{
public:
	SineGenerator(float frequency) : AudioFilter(pAudioFilter()), ToggleableFilter(), frequency_(frequency), time_(0.0f)
	{

	}

	error_type_t do_process(audio_buffer_t& buffer) override
	{
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

class SquareAdder : public AudioFilter, public ToggleableFilter
{
public:
	SquareAdder(const pAudioFilter& child, float frequency) : AudioFilter(child), frequency_(frequency), time_(0.0f)
	{

	}

	error_type_t do_process(audio_buffer_t& buffer) override
	{
		if (!is_enabled()) return error_type_t::ok;

		const double step = 1.0 / convert_rate_to_int(buffer.params.rate);

		for (auto& sample : buffer.data) {
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

class Control : public SDLDevice, public AudioFilter
{
public:
	Control(const pAudioFilter& child, int width, int height, int instruments, int steps, float loop_length) : SDLDevice(width, height, "Sequencer"), 
		AudioFilter(child), instruments_(instruments), steps_(steps), last_step_(-1), loop_length_(loop_length), time_(0.0f), data_(width, height)
	{
		sequence_.resize(instruments * steps, false);

		start();
	}

	~Control()
	{
		stop();
	}

private:
	int instruments_;
	int steps_;
	int last_step_;
	double loop_length_;
	double time_;

	/// Video data
	iimavlib::video_buffer_t data_;
	/// Sequence data
	std::vector<bool> sequence_;

	void draw_sequence()
	{
		const rgb_t enabled_color = {20, 200, 50};
		const rgb_t disabled_color = {20, 20, 20};

		const int step_size = data_.size.width / steps_;
		const int inst_size = data_.size.height / instruments_;

		double loop_fraction = time_ / loop_length_;

		for (int i = 0; i < steps_; ++i)
		{
			for (int j = 0; j < instruments_; ++j)
			{
				const rectangle_t rect = {i * step_size, j * inst_size, step_size, inst_size};
				draw_rectangle(data_, rect, sequence_[i * instruments_ + j] ? enabled_color : disabled_color);
			}
		}

		draw_line(data_, rectangle_t(static_cast<int>(loop_fraction * data_.size.width), 0), rectangle_t(static_cast<int>(loop_fraction * data_.size.width), data_.size.height), rgb_t(255, 255, 0));
		blit(data_);
	}

	inline int current_step()
	{
		return static_cast<int>(steps_ * time_ / loop_length_);
	}

	error_type_t do_process(audio_buffer_t& buffer) override
	{
		if (is_stopped()) return error_type_t::failed;
		// Not touching the data, simply passing it through
		// But update graphics

		time_ += buffer.valid_samples * 1.0 / convert_rate_to_int(buffer.params.rate);
		if (time_ > loop_length_) time_ -= loop_length_;
		draw_sequence();

		int cur_step = current_step();
		if (cur_step != last_step_)
		{
			for (int i = 0; i < instruments_; ++i)
			{
				std::shared_ptr<ToggleableFilter> filter = std::dynamic_pointer_cast<ToggleableFilter>(get_child(i));
				filter->set_enabled(sequence_[cur_step * instruments_ + i]);
			}
			last_step_ = cur_step;
		}

		return error_type_t::ok;
	}


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
		.add<Control>(800, 200, 2, 20, 5.0f)
		.add<iimavlib::PlatformSink>(device_id)
		.sink();

	sink->run();
}
catch (std::exception& e) {
	using namespace iimavlib;
	logger[log_level::fatal] << "The application ended unexpectedly with an error: " << e.what();
}