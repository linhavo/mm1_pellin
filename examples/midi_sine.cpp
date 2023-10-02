/**
 * @file 	playback_sine_high.cpp
 *
 * @date 	1.10.2023
 * @authors Zdenek Travnicek <travnicek@iim.cz>
 * 			Ondrej Slaby <slaby@iim.cz>
 * @copyright GNU Public License 3.0
 *
 * Sample program generating sine based on MIDI input and playing it out using high level API.
 */


#include "iimavlib.h"
#include "iimavlib/WaveSource.h"
#include "iimavlib/WaveSink.h"
#include "iimavlib/Utils.h"
#include "iimavlib/AudioFilter.h"
#include "iimavlib/midi/MidiDevice.h"
#include "iimavlib/midi/MidiTypes.h"
#include <string>
#include <cassert>
#include <cmath>

using namespace iimavlib;
using namespace midi;

namespace {
// Max value for int16_t
const double max_val = std::numeric_limits<int16_t>::max();

// Value of 2*PI
const double pi2 = 8.0*std::atan(1.0);
}

class SineGenerator: public AudioFilter, Midi
{
public:
	SineGenerator():AudioFilter(pAudioFilter()),
	frequency_(440), time_(0.0), amplitude_(0.0)
{
	Midi::start();
	Midi::open_all_inputs();
}
private:
	void on_noteon(const note_t &note) override
	{
		// First convert the received note to a frequency value
		frequency_ = note_to_freq(note.note);
		// Reset generator time
		time_ = 0.0;
		// Use the note event's velicoty to decide how loud the sine should be
		amplitude_ = note.velocity / 127.f;

		logger[log_level::info] << "Note: " << static_cast<int>(note.note) << ", Frequency: " << frequency_;
	}

	void on_noteoff(const note_t &) override
	{
		// When a note stops playing, turn the amplitude down to generate silence.
		// Note that this doesn't account for simultaneous or staggered keypresses.
		amplitude_ = 0.0;
	}

	error_type_t do_process(audio_buffer_t& buffer) override
	{
		// Prepare few values to save typing (and enable some optimizations)
		const double step = 1.0 / convert_rate_to_int(buffer.params.rate);

		for (auto& sample: buffer.data) {
			sample = static_cast<int16_t>(max_val * std::sin(time_ * frequency_ * pi2) * amplitude_);
			time_=time_ + step;
		}
		buffer.valid_samples = buffer.data.size();
		return error_type_t::ok;
	}

	double frequency_;
	double time_;
	double amplitude_;
};

int main(int argc, char** argv) try
{
	logger[log_level::debug] << "Generating sine with frequency based on midi input.";

	audio_id_t device_id = PlatformDevice::default_device();
	if (argc>1) {
		device_id = simple_cast<audio_id_t>(argv[2]);
	}
	logger[log_level::debug] << "Using audio device " << device_id;

	/* ******************************************************************
	 *                      Create and run the filter chain
	 ****************************************************************** */

	// Create filter chain
	auto chain = filter_chain<SineGenerator>()
						.add<WaveSink>("xx.wav")
						.add<PlatformSink>(device_id)
						.sink();

	// Start the filters
	chain->run();

	/*
	 * Alternative syntax would be:
	 * auto sine = std::make_shared<SineGenerator>();
	 * auto sink = std::make_shared<PlatformSink>(sine, device_id);
	 * sink->run();
	 *
	 */


}
catch (std::exception& e)
{
	logger[log_level::fatal] << "ERROR: An error occurred during program run: " << e.what();
}
