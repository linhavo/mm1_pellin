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
#include "iimavlib/filters/SimpleEchoFilter.h"

#include "iimavlib/midi/MidiDevice.h"
#include "iimavlib/midi/MidiTypes.h"

#include <iimavlib/SDLDevice.h>
#include <iimavlib/AudioFilter.h>
#include <iimavlib_high_api.h>
#include <iimavlib/video_ops.h>
#include <iimavlib/Utils.h>
#include <iimavlib/keys.h>
#include <mutex>
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>

#include <complex>
#include <vector>



#include <algorithm>
#include <atomic>
#include <functional>
#include "../src/video_ops.cpp"

using namespace iimavlib;


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

class MySimpleEchoFilter : public AudioFilter, public ToggleableFilter
{
public:
	MySimpleEchoFilter(const pAudioFilter& child, double delay, double decay)
		:AudioFilter(child), delay_(delay), decay_(decay)
	{

	}
	MySimpleEchoFilter::~MySimpleEchoFilter()
	{

	}
	/**
	 * Adds 'echo' to the @em dest buffer, by adding weighted values from @em src.
	 * @tparam T type of the samples. Should be signed.
	 * @param dest Destination buffer to add echo to.
	 * @param src Source buffer to read old values for echo.
	 * @param count Number of samples to read.
	 * @param decay Weighting of old and new values.
	 */
	template<typename T>
	void add_echo(T dest, T src, size_t count, double decay)
	{

		for (size_t sample = 0; sample < count; ++sample) {
			*dest = (decay * *src++) + ((1.0 - decay) * *dest);
			dest++;
		}
	}
private:
	std::vector<audio_sample_t> old_samples_;
	double delay_;
	double decay_;
	double time_;
	error_type_t MySimpleEchoFilter::do_process(audio_buffer_t& buffer)
	{
		if (!is_enabled())
			return error_type_t::ok;
		// Return OK for empty buffer - nothing to do here
		if (buffer.valid_samples == 0) return error_type_t::ok;

		// Some constant values that is convenient to have prepared
		const size_t frequency = convert_rate_to_int(buffer.params.rate);
		const size_t delay_samples = static_cast<size_t>(frequency * delay_);


		// Make sure old_samples_ is large enough
		old_samples_.resize(delay_samples, 0);

		// Pointer to the buffer after type conversion to int16_t
		const auto data = buffer.data.begin();

		// Calculate how many samples from old_samples we're gonna use
		const size_t from_old = std::min(buffer.valid_samples, delay_samples);

		// And add echo to them (from old_samples_)
		add_echo(data, old_samples_.begin(), from_old, decay_);

		// if buffer.valid_samples is lesser or equal than delay_samples, we already have processed all the samples
		if (buffer.valid_samples <= delay_samples) {
			// Move unused part of the buffer to the beginning
			std::copy(old_samples_.begin() + from_old, old_samples_.end(), old_samples_.begin());
			// Resize old_samples_ to the size of valid data in it
			old_samples_.resize(std::distance(old_samples_.begin() + from_old, old_samples_.end()));
			// And insert samples from processed buffer
			old_samples_.insert(old_samples_.end(), data, data + buffer.valid_samples);
		}
		else {
			// Our input buffer was larger then old_samples_, that means we still need to add echo to some samples
			//logger[log_level::debug] << "Too large input buffer or too small delay, not tested";
			// Add echo to the rest of input buffer
			add_echo(data + from_old, data, (buffer.valid_samples - from_old), decay_);
			// We have no valid old samples
			old_samples_.resize(0);
			// Copy samples to old_samples_ from input buffer
			old_samples_.insert(old_samples_.end(),
				data + (buffer.valid_samples - delay_samples),
				data + buffer.valid_samples);
		}
		return error_type_t::ok;
	};
	void reinitialize() override
	{
		time_ = 0.0f;
	}
};
class MySimpleReverbFilter : public AudioFilter, public ToggleableFilter
{
public:
	MySimpleReverbFilter(const pAudioFilter& child, double reverb_factor, double decay)
		: AudioFilter(child), reverb_factor_(reverb_factor), decay_(decay)
	{

	}

	~MySimpleReverbFilter()
	{
			
	}

	template<typename T>
	void add_reverb(T dest, T src, size_t count, double decay, audio_buffer_t& buffer)
	{
		//const double feedback1 = 0.6;
		//const double feedback2 = 0.3;
		//const double feedback3 = 0.1;
		//const size_t delay1 = static_cast<size_t>(convert_rate_to_int(buffer.params.rate) * 0.02);
		//const size_t delay2 = static_cast<size_t>(convert_rate_to_int(buffer.params.rate) * 0.04);
		//const size_t delay3 = static_cast<size_t>(convert_rate_to_int(buffer.params.rate) * 0.06);

		//for (size_t sample = 0; sample < count; ++sample) {
		//	// Delay Line 1
		//	const audio_sample_t delayed1 = old_samples_[0];
		//	old_samples_[0] = *src + feedback1 * delayed1;

		//	// Delay Line 2
		//	const audio_sample_t delayed2 = old_samples_[1];
		//	old_samples_[1] = delayed1 + feedback2 * delayed2;

		//	// Delay Line 3
		//	const audio_sample_t delayed3 = old_samples_[2];
		//	old_samples_[2] = delayed2 + feedback3 * delayed3;

		//	// Combine delayed samples and apply decay
		//	*dest = (decay * (*src++ + delayed1 + delayed2 + delayed3)) + ((1.0 - decay) * *dest);
		//	dest++;
		//}
		const double feedback = 0.1; // Adjust as needed
		const size_t delay1 = static_cast<size_t>(convert_rate_to_int(buffer.params.rate) * 0.03); // Adjust delay times
		const size_t delay2 = static_cast<size_t>(convert_rate_to_int(buffer.params.rate) * 0.05);

		for (size_t sample = 0; sample < count; ++sample) {
			// Delay Line 1
			const audio_sample_t delayed1 = old_samples_[0];
			old_samples_[0] = *src + feedback * delayed1;

			// Delay Line 2
			const audio_sample_t delayed2 = old_samples_[1];
			old_samples_[1] = delayed1 + feedback * delayed2;

			// Combine delayed samples and apply decay
			*dest = (decay * (*src++ + delayed1 + delayed2)) + ((1.0 - decay) * *dest);
			dest++;
		}
	}

private:
	std::vector<audio_sample_t> old_samples_;
	double decay_;
	double reverb_factor_;
	double time_;


	error_type_t MySimpleReverbFilter::do_process(audio_buffer_t& buffer) override
	{
		if (!is_enabled())
			return error_type_t::ok;

		if (buffer.valid_samples == 0)
			return error_type_t::ok;

		const size_t frequency = convert_rate_to_int(buffer.params.rate);
		const size_t delay_samples = static_cast<size_t>(frequency * reverb_factor_);

		old_samples_.resize(delay_samples, 0);

		const auto data = buffer.data.begin();

		const size_t from_old = std::min(buffer.valid_samples, delay_samples);

		add_reverb(data, old_samples_.begin(), from_old, decay_,  buffer);

		if (buffer.valid_samples <= delay_samples) {
			std::copy(old_samples_.begin() + from_old, old_samples_.end(), old_samples_.begin());
			old_samples_.resize(std::distance(old_samples_.begin() + from_old, old_samples_.end()));
			old_samples_.insert(old_samples_.end(), data, data + buffer.valid_samples);
		}
		else {
			add_reverb(data + from_old, data, (buffer.valid_samples - from_old), decay_,  buffer);
			old_samples_.resize(0);
			old_samples_.insert(old_samples_.end(),
				data + (buffer.valid_samples - delay_samples),
				data + buffer.valid_samples);
		}

		return error_type_t::ok;
	}
	void reinitialize() override
	{
		time_ = 0.0f;
	}
	
};

//template <class T>
//simplearray_t<T> IDFT1D(const complexarray_t<T>& ab) {
//	const auto N = ab.size();
//
//	if (N == 0 || (N & (N - 1))) {
//		throw std::runtime_error("IDFT Error: the input number of samples must be a power of 2!");
//	}
//	else if (N <= 8) {
//		// Assuming you have a DFT1D function for the base case
//		// Replace with your actual implementation of IDFT1D for N <= 8
//		// return DFT1D(ab);
//		simplearray_t<T> realInput;
//		realInput.reserve(N * 2);
//		for (const auto& val : ab) {
//			realInput.push_back(val.real());
//			realInput.push_back(val.imag());
//		}
//		AudioFFT<float> fft;
//		std::vector<audio_sample_t> s = realInput;
//		simplearray_t<T> result = fft.FFT1D(s.begin(), s.end()); 
//
//
//		return result;
//	}
//	else {
//		complexarray_t<T> coefficients_odd;
//		coefficients_odd.reserve(N / 2);
//		for (auto i = 0u; i < N; i += 2)
//			coefficients_odd.push_back(ab[i]);
//		auto samples_odd = IDFT1D(coefficients_odd);
//
//		complexarray_t<T> coefficients_even;
//		coefficients_even.reserve(N / 2);
//		for (auto i = 1u; i < N; i += 2)
//			coefficients_even.push_back(ab[i]);
//		auto samples_even = IDFT1D(coefficients_even);
//
//		complexarray_t<T> f;
//		for (auto n = 0u; n < N; n++) {
//			f.push_back(std::exp(std::complex<T>(0, 2 * M_PI * n / N))); // Corrected the sign
//		}
//
//		simplearray_t<T> result(N, 0);
//		for (auto i = 0u; i < N / 2; ++i) {
//			result[i] = samples_odd[i] + f[i].real() * samples_even[i];
//			result[i + N / 2] = samples_odd[i] - f[i + N / 2].real() * samples_even[i]; // Change the sign
//		}
//		return result;
//	}
//}
//
//template <class T>
//simplearray_t<T> IFFT1D(const complexarray_t<T>& ab) {
//	const auto N = ab.size();
//
//	if (N == 0 || (N & (N - 1))) {
//
//		throw std::runtime_error("IFFT Error: the input number of samples must be a power of 2!");
//	}
//	else if (N <= 8) {
//		// Assuming you have a DFT1D function for the base case
//		return IDFT1D(ab);
//	}
//	else {
//
//		complexarray_t<T> coefficients_odd;
//		coefficients_odd.reserve(N / 2);
//		for (auto i = 0u; i < N; i += 2)
//			coefficients_odd.push_back(ab[i]);
//
//		simplearray_t<T> samples_odd = IFFT1D(coefficients_odd);
//
//		complexarray_t<T> coefficients_even;
//		coefficients_even.reserve(N / 2);
//		for (auto i = 1u; i < N; i += 2)
//			coefficients_even.push_back(ab[i]);
//		auto samples_even = IFFT1D(coefficients_even);
//
//		complexarray_t<T> f;
//		for (auto n = 0u; n < N; n++) {
//			f.push_back(std::exp(std::complex<T>(0, -2 * M_PI * static_cast<T>(n) / N))); // Corrected the sign and type
//		}
//
//
//
//		simplearray_t<T> result(N, 0);
//		for (auto i = 0u; i < N / 2; ++i) {
//			result[i] = samples_odd[i] + f[i].real() * samples_even[i];
//			result[i + N / 2] = samples_odd[i] + f[i + N / 2].real() * samples_even[i];
//		}
//		return result;
//	}
//}
//
//class MySimpleLowPassFilter : public AudioFilter, public ToggleableFilter
//{
//public:
//	MySimpleLowPassFilter(const pAudioFilter& child, double lfoFrequency, double maxCutoff)
//		: AudioFilter(child), lfoFrequency_(lfoFrequency), maxCutoff_(maxCutoff)
//	{
//		float timespec_ = 10.0f / 1000.0f;
//
//		const audio_params_t& params = get_params();
//		cache_size_ = static_cast<size_t>(timespec_ * convert_rate_to_int(params.rate));
//		cache_size_ = static_cast<size_t>(pow(2, ceil(log2(cache_size_))) * 2);
//		logger[log_level::info] << "Cache size: " << cache_size_;
//		
//		sample_cache_.resize(cache_size_);
//		time_ = 0.0;
//		last_sample_ = 0.0;
//		changed_ = false;
//	}
//
//	~MySimpleLowPassFilter()
//	{
//	}
//
//private:
//	double lfoFrequency_;
//	double maxCutoff_;
//	AudioFFT<float> fft;
//	double time_;
//	std::vector<audio_sample_t> sample_cache_;
//	
//	std::atomic<bool> end_;
//	std::atomic<bool> changed_;
//	size_t last_sample_;
//	size_t cache_size_;
//	std::vector<complexarray_t<float>> coefficient_array_entire;
//
//	void cleanupFilter()
//	{
//		// Cleanup if needed
//	}
//
//	void update_cache(const audio_buffer_t& buffer)
//	{
//		const audio_sample_t* src = &buffer.data[0];
//		size_t src_remaining = buffer.valid_samples;
//
//		while (src_remaining)
//		{
//			const size_t to_copy = std::min(src_remaining, sample_cache_.size() - last_sample_);
//			std::copy_n(src, to_copy, &sample_cache_[0] + last_sample_);
//			last_sample_ += to_copy;
//			if (last_sample_ >= sample_cache_.size())
//				last_sample_ = 0;
//			src_remaining -= to_copy;
//		}
//		changed_.store(true);
//	}
//
//
//	complexarray_t<float> applyLowPassFilter(complexarray_t<float>& spectrum)
//	{
//		// Implement your low-pass filter logic here
//		// You can modulate the cutoff frequency based on the LFO and maxCutoff parameters
//		// Modify the spectrum in-place
//		// Example: simple low-pass filter with a linear cutoff modulation
//		double lfoValue = std::sin(2.0 * M_PI * time_ * lfoFrequency_);
//		double cutoff = maxCutoff_ * (lfoValue + 1.0) / 2.0;
//
//		for (size_t i = 0; i < spectrum.size(); ++i)
//		{
//			// Implement your low-pass filter logic here
//			// You may want to design a more sophisticated filter based on your requirements
//			if (i > cutoff)
//			{
//				spectrum[i] = 0.0;
//			}
//		}
//		return spectrum;
//	}
//
//	error_type_t do_process(audio_buffer_t& buffer) override
//	{
//
//		if (!is_enabled())
//			return error_type_t::ok;
//
//		// Return OK for an empty buffer - nothing to do here
//		if (buffer.valid_samples == 0)
//			return error_type_t::ok;
//
//
//		const size_t frequency = convert_rate_to_int(buffer.params.rate);
//		const size_t N = buffer.valid_samples;
//
//		update_cache(buffer);
//
//		time_ += buffer.valid_samples * 1.0 / convert_rate_to_int(buffer.params.rate);
//
//		// Convert audio samples to a complex spectrum
//		complexarray_t<float> coefficient_array;
//		{
//			coefficient_array = fft.FFT1D(sample_cache_.begin(), sample_cache_.end());
//		}
//		// Apply the low-pass filter to the spectrum
//
//		coefficient_array = applyLowPassFilter(coefficient_array);
//		// Perform IFFT
//
//		simplearray_t<float> result = IFFT1D(coefficient_array);
//		std::cout << "jsem tady\n\n\n";;
//
//		// Copy the modified samples back to the audio buffer
//		for (size_t i = 0; i < N; ++i)
//		{
//			buffer.data[i] = static_cast<audio_sample_t>(result[i]);
//		}
//
//		return error_type_t::ok;
//	}
//
//	void reinitialize() override
//	{
//		time_ = 0.0;
//	}
//};




namespace
{
	// Max value for int16_t
	const double max_val = std::numeric_limits<int16_t>::max();

	// Value of 2*PI
	const double pi2 = 8.0 * std::atan(1.0);
}



class MIDIFrequencyGenerator : public AudioFilter, public midi::Midi
{
public:
	MIDIFrequencyGenerator() : AudioFilter(pAudioFilter()),  frequency_(880), time_(0.0), amplitude_(10.0)
	{
		midi::Midi::start();
		midi::Midi::open_all_inputs();
	
	}
	~MIDIFrequencyGenerator()
	{

		midi::Midi::stop();
	}

	error_type_t do_process(audio_buffer_t& buffer) override
	{
		// If disabled, clear the output buffer


		const double step = 1.0 / convert_rate_to_int(buffer.params.rate);

		for (auto& sample : buffer.data)
		{
			sample = static_cast<int16_t>(max_val * std::sin(time_ * frequency_ * pi2));
			time_ = time_ + step;
		}
		buffer.valid_samples = buffer.data.size();
		return error_type_t::ok;
	}
private:

	/// MIDI ---
	/// Mutex to lock @em index_ and @em position_
	std::mutex position_mutex_;
	/*/// Index of currently playing drum
	int index_;
	/// Next sample to be played for current drum
	size_t position_;*/

	double frequency_;
	double amplitude_;
	double time_;



	// MIDI ---

	/**
	 * Overloaded method for control event handling
	*/
	void on_control(const midi::control_t& control) {
		// Play drum 2 on any control event (these are usually many in sequence, so not the best for directly starting playback)
		logger[log_level::info] << "Control: " << static_cast<int>(control.channel) << ", " << static_cast<int>(control.param) << ", " << static_cast<int>(control.value);
		logger[log_level::info] << "Drum 2";
		std::unique_lock<std::mutex> lock(position_mutex_); // Lock the variables
		if (control.channel > 13)
		{
			frequency_ = control.value;
		}
		else {
			amplitude_ = control.value;
		}
		// Playing from sdl_drums_midi.cpp
		// index_ = 2;
		//position_ = 0;
	}
};

/**
 * Simple sine wave generator. See the playback_sine.cpp example for details.
 * This class additionally implements disabling, which means we have to clear
 * the sample buffer when not generating, otherwise it will contain old/garbage data.
 */
class SineGenerator : public AudioFilter, public ToggleableFilter
{
public:
	SineGenerator(const pAudioFilter& child, float frequency) : AudioFilter(pAudioFilter()), ToggleableFilter(), frequency_(frequency), time_(0.0f)
	{
	}

	error_type_t do_process(audio_buffer_t& buffer) override
	{
		// If disabled, clear the output buffer
		if (!is_enabled())
		{
			/*for (auto& sample : buffer.data)
			{
				sample = 0;
			}*/
			return error_type_t::ok;
		}

		const double step = 1.0 / convert_rate_to_int(buffer.params.rate);

		for (auto& sample : buffer.data)
		{
			sample += static_cast<int16_t>(max_val * std::sin(time_ * frequency_ * pi2));
			//sample += static_cast<int16_t>(std::copysignl(max_val, std::sin(time_ * frequency_ * pi2)));

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

class SawtoothWaveGenerator : public AudioFilter, public ToggleableFilter
{
public:
	SawtoothWaveGenerator(float frequency) : AudioFilter(pAudioFilter()), ToggleableFilter(), frequency_(frequency), time_(0.0f)
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

		for (auto& sample : buffer.data)
		{
			// Generate a sawtooth wave sample
			sample = static_cast<int16_t>(max_val * (2.0 * (time_ * frequency_ - std::floor(time_ * frequency_ + 0.5))));
			time_ = std::fmod(time_ + step, 1.0 / frequency_);
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
		if (!is_enabled())
			return error_type_t::ok;

		const double step = 1.0 / convert_rate_to_int(buffer.params.rate);

		for (auto& sample : buffer.data)
		{
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

class SawtoothAdder : public AudioFilter, public ToggleableFilter
{
public:
	SawtoothAdder(const pAudioFilter& child, float frequency) : AudioFilter(child), frequency_(frequency), time_(0.0f)
	{
	}

	error_type_t do_process(audio_buffer_t& buffer) override
	{
		// If disabled, just skip processing and leave existing data alone
		if (!is_enabled())
			return error_type_t::ok;

		const double step = 1.0 / convert_rate_to_int(buffer.params.rate);

		for (auto& sample : buffer.data)
		{
			// Instead of overwriting sample data, add our own generated signal to it, so the original
			// signal doesn't get lost.
			sample += static_cast<int16_t>(std::copysignl(max_val, sawtooth_wave(time_, frequency_)));
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

	// Function to generate a sawtooth wave
	double sawtooth_wave(double time, double frequency)
	{
		// Implement the sawtooth wave generation logic here
		// You can use different mathematical formulas to generate a sawtooth wave
		// For example, you can use the modulus function to create a sawtooth shape
		return 2.0 * (time * frequency - std::floor(time * frequency + 0.5));
	}
};

class TriangleAdder : public AudioFilter, public ToggleableFilter
{
public:
	TriangleAdder(const pAudioFilter& child, float frequency) : AudioFilter(child), frequency_(frequency), time_(0.0f)
	{
	}

	error_type_t do_process(audio_buffer_t& buffer) override
	{
		// If disabled, just skip processing and leave existing data alone
		if (!is_enabled())
			return error_type_t::ok;

		const double step = 1.0 / convert_rate_to_int(buffer.params.rate);

		for (auto& sample : buffer.data)
		{
			// Instead of overwriting sample data, add our own generated signal to it, so the original
			// signal doesn't get lost.
			sample += static_cast<int16_t>(std::copysignl(max_val, triangle_wave(time_, frequency_)));
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

	// Function to generate a triangle wave
	double triangle_wave(double time, double frequency)
	{
		// Implement the triangle wave generation logic here
		// You can use different mathematical formulas to generate a triangle wave
		// For example, you can use the modulus function to create a triangle shape
		return std::fmod(2.0 * time * frequency, 2.0) - 1.0;
	}
};

/**
 * Example class facilitating the control of two separate signal generators (appended above this controller in the chain).
 * The controller accesses its instruments by going up the filter chain. It expects the specified number of generators set
 * in the constructor to implement the ToggleableFilter interface. It then uses these interfaces to enable/disable generators
 * as per user input on the current timeline.
 */
class Control : public SDLDevice, public midi::Midi, public AudioFilter
{
public:
	Control(const pAudioFilter& child, int width, int height, int instruments, int steps, float loop_length) : SDLDevice(width, height, "Sequencer"),
		AudioFilter(child), instruments_(instruments), steps_(steps), last_step_(-1), loop_length_(loop_length), time_(0.0f), data_(width, height)
	{
		timespec_ = 10.0f / 1000.0f;
		sequence_.resize(instruments * steps, false);

		const audio_params_t& params = get_params();
		cache_size_ = static_cast<size_t>(timespec_ * convert_rate_to_int(params.rate));
		cache_size_ = static_cast<size_t>(pow(2, ceil(log2(cache_size_))) * 2);
		logger[log_level::info] << "Cache size: " << cache_size_;
		sample_cache_.resize(cache_size_);
		barwidth = 40;
		x_count = 0;
		height_ = height / 2;
		width_ = width;

		thread_ = std::thread(std::bind(&Control::execute_thread, this));
		time_elapsed = 0.000f;
		const auto max_int16_value = std::numeric_limits<int16_t>::max();

		magic_constant = 1.0f / 16384.0f / max_int16_value;

		SDLDevice::start();

		// MIDI
	}

	~Control()
	{
		SDLDevice::stop();
	}

private:
	std::vector<audio_sample_t> old_samples_;

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

	void execute_thread()
	{
		logger[log_level::debug] << "Drawing thread started";
		while (!end_)
		{
			if (changed_)
			{
				draw_wave();
				draw_sequence();
			}
			if (!blit(data_))
			{
				logger[log_level::debug] << "Drawing thread finishing";
				end_ = true;
			}
		}
		auto surface = SDL_GetVideoSurface();
		SDL_SaveBMP(surface, "screenshot.bmp");
	}

	void update_cache(const audio_buffer_t& buffer)
	{
		std::unique_lock<std::mutex> lock(mutex_);
		const audio_sample_t* src = &buffer.data[0];
		size_t src_remaining = buffer.valid_samples;

		while (src_remaining)
		{
			const size_t to_copy = std::min(src_remaining, sample_cache_.size() - last_sample_);
			std::copy_n(src, to_copy, &sample_cache_[0] + last_sample_);
			last_sample_ += to_copy;
			if (last_sample_ >= sample_cache_.size())
				last_sample_ = 0;
			src_remaining -= to_copy;
		}
		changed_.store(true);
	}

	
	//void add_echo(std::vector<audio_sample_t>::iterator dest, std::vector<audio_sample_t>::iterator src, size_t count, double decay)
	//{
	//	for (size_t sample = 0; sample < count; ++sample) {
	//		*dest = (decay * *src++) + ((1.0 - decay) * *dest);
	//		dest++;
	//	}
	//}

	void draw_wave()
	{
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

		for (int y = 0; y < height_; ++y)
		{
			const size_t coefficient_number = y * unique_coefficients / height_;

			// Get value for the coefficient
			auto coefficient = std::abs(coefficient_array[coefficient_number]) * magic_constant;
			coefficient *= 100;

			auto yycol = static_cast<int>(255 * (coefficient));

			std::vector<int> colr = { 1, 0, 0 };
			colr[0] = int(yycol);
			colr[2] = int(50 * y / height_);
			for (auto i = 0; i < 3; i++)
			{
				colr[i] += int(yycol ^ 2);
				if (colr[i] > 255)
				{
					colr[i] = 255;
				}
			}
			draw_bars(static_cast<int>(loop_fraction * data_.size.width), y, colr);
		}
		// sample_cache_.clear();
	}

	void draw_bars(int x, int y, std::vector<int> colr)
	{
		rectangle_t rectangle = intersection(data_.size, rectangle_t(x, y, barwidth / 20, (data_.size.height / 2) - y));
		iimavlib::draw_rectangle(data_, rectangle_t(rectangle.x, rectangle.y, rectangle.width, 3), rgb_t(colr[0], colr[1], colr[2]));
	}

	void draw_sequence()
	{
		// Set up our display colors
		const rgb_t enabled_color = { 20, 200, 50 };
		const rgb_t enabled_color_delay = { 200, 10, 100 };
		const rgb_t enabled_color_reverb = { 0, 100, 200 };


		const rgb_t disabled_color = { 20, 20, 20 };

		// What's the size of each sequencer step (width) and of each instrument row (height)
		const int step_size = data_.size.width / steps_;
		const int inst_size = (data_.size.height / 2) / instruments_;

		// How far we are through the current loop
		double loop_fraction = time_ / loop_length_;

		// Draw all rectangles representing each sequencer step + instrument
		for (int i = 0; i < steps_; ++i)
		{
			rectangle_t rect = { i * step_size, (0 * inst_size) + (data_.size.height / 2), step_size, inst_size };
			draw_rectangle(data_, rect, sequence_[i * instruments_ + 0] ? enabled_color_delay : disabled_color);

			rect = { i * step_size, (1 * inst_size) + (data_.size.height / 2), step_size, inst_size };
			draw_rectangle(data_, rect, sequence_[i * instruments_ + 1] ? enabled_color_reverb : disabled_color);
			for (int j = 2; j < instruments_; ++j)
			{
				rect = { i * step_size, (j * inst_size) + (data_.size.height / 2), step_size, inst_size };
				draw_rectangle(data_, rect, sequence_[i * instruments_ + j] ? enabled_color : disabled_color);
			}
		}

		// Draw a moving cursor to indicate current progress through the loop
		draw_line(data_, rectangle_t(static_cast<int>(loop_fraction * data_.size.width), (data_.size.height / 2)), rectangle_t(static_cast<int>(loop_fraction * data_.size.width), (data_.size.height)), rgb_t(255, 255, 0));
		// blit(data_);
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
		if (SDLDevice::is_stopped())
			return error_type_t::failed;
		// Not touching the data, simply passing it through
		// But update graphics and control generators

		// Update our loop time (and loop it if appropriate)
		time_ += buffer.valid_samples * 1.0 / convert_rate_to_int(buffer.params.rate);
		if (time_ > loop_length_)
			time_ -= loop_length_;

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

	// /**
	//  * Overrides the key press handling method. We have to handle a quit key ourselves (or pass it to the parent class).
	//  * Otherwise, when a specific key is pressed, toggle a generator at the current sequencer step.
	//  */
	// bool do_key_pressed(const int key, bool pressed) override
	// {
	// 	if (!pressed)
	// 		return true;
	// 	if (keys::key_q == key)
	// 		return false;

	// 	int filter_index = std::max(std::min(instruments_, (key - keys::key_a)), 0);
	// 	int cur_step = current_step();
	// 	sequence_[cur_step * instruments_ + filter_index] = !sequence_[cur_step * instruments_ + filter_index];

	// 	return true;
	// }

	bool do_key_pressed(const int key, bool pressed) override
	{
		if (!pressed)
			return true;
		if (keys::key_q == key)
			return false;

		// Define the QWERTZ layout
		std::vector<int> qwertz_keys = { keys::key_w, keys::key_e, keys::key_r, keys::key_t, keys::key_z, keys::key_u, keys::key_i, keys::key_o, keys::key_p };

		// Find the key in the QWERTZ layout
		auto it = std::find(qwertz_keys.begin(), qwertz_keys.end(), key);

		// If the key is not in the QWERTZ layout, ignore it
		if (it == qwertz_keys.end())
			return true;

		// Calculate the filter index based on the position of the key in the QWERTZ layout
		int filter_index = std::max(std::min(instruments_, static_cast<int>(std::distance(qwertz_keys.begin(), it))), 0);
		//std::cout << filter_index << "\n";
		int cur_step = current_step();
		if (filter_index == 0) {
			for (int i = 0; i < steps_; i++) {
				sequence_[i * instruments_ + filter_index] = !sequence_[i * instruments_ + filter_index];
			}
			return true;
		}
		if (filter_index == 1) {
			for (int i = 0; i < steps_; i++) {
				sequence_[i * instruments_ + filter_index] = !sequence_[i * instruments_ + filter_index];
			}
			return true;
		}
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
	if (argc > 1)
	{
		device_id = iimavlib::simple_cast<iimavlib::audio_id_t>(argv[1]);
	}

	auto sink = iimavlib::filter_chain<MIDIFrequencyGenerator>()

		.add<SineGenerator>(440.0)
		.add<SquareAdder>(580.0)
		// .add<TriangleAdder>(380.0)
		// .add<SawtoothAdder>(120.0)
		.add<TriangleAdder>(100.0)
		.add<MySimpleEchoFilter>(1.0, 0.1)


		.add<MySimpleReverbFilter>(0.1, 0.25)
		//.add<MySimpleLowPassFilter>(80.0, 40.0)



		.add<Control>(800, 400, 5, 16, 5.0f)

		.add<iimavlib::PlatformSink>(device_id)
		.sink();

	sink->run();
}
catch (std::exception& e)
{
	using namespace iimavlib;
	logger[log_level::fatal] << "The application ended unexpectedly with an error: " << e.what();
}