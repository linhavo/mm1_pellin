#include "iimavlib/midi/MidiWinMM.h"
#include "iimavlib/Utils.h"
#include <functional>

namespace iimavlib
{
namespace midi
{

namespace
{
void CALLBACK win_mm_mim_callback(HMIDIIN /* hMidiIn */, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR /* dwParam2 */)
{
	MidiWinMM *midi = reinterpret_cast<MidiWinMM*>(dwInstance);
	if (wMsg == MIM_DATA) {
		midi->enqueue_midi(dwParam1);
	}
}
}

MidiWinMM::MidiWinMM(const std::string&) : finish_(false)
{
	// Cannot name our MIDI device in windows - ignore the supplied name.
}

MidiWinMM::~MidiWinMM()
{
	for (HMIDIIN handle : open_inputs_)
	{
		midiInStop(handle);
		midiInClose(handle);
	}
	open_inputs_.clear();

	for (HMIDIOUT handle : open_outputs_)
	{
		midiOutClose(handle);
	}
	open_outputs_.clear();
}

void MidiWinMM::open_all_inputs()
{
	UINT dev_count = midiInGetNumDevs();
	logger[log_level::info] << "Opening all MIDI inputs: " << dev_count;
	for (UINT i = 0; i < dev_count; ++i)
	{
		open_input(i);
	}
}

void MidiWinMM::open_input(const midi_id_t& device)
{
	HMIDIIN handle;
	midiInOpen(&handle, device, reinterpret_cast<DWORD_PTR>(&win_mm_mim_callback), reinterpret_cast<DWORD_PTR>(this), CALLBACK_FUNCTION);
	midiInStart(handle);
	open_inputs_.insert(handle);
}

void MidiWinMM::open_output(const midi_id_t& device)
{
	HMIDIOUT handle;
	midiOutOpen(&handle, device, NULL, reinterpret_cast<DWORD_PTR>(this), CALLBACK_NULL);
	open_outputs_.insert(handle);
}

void MidiWinMM::start()
{
	std::unique_lock<std::mutex> lock(thread_mutex_);
	if (thread_.joinable()) return;
	thread_ = std::thread(std::bind(&MidiWinMM::run, this));
	logger[log_level::debug] << "MIDI thread started";
	if (thread_.joinable()) return;
}

void MidiWinMM::stop()
{
	std::unique_lock<std::mutex> lock(thread_mutex_);
	if (!thread_.joinable()) return;
	finish_ = true;
	thread_.join();

	logger[log_level::debug] << "MIDI thread joined";
}

bool MidiWinMM::is_stopped() const
{
	return finish_;
}

void MidiWinMM::send_noteon(const note_t& note)
{
	DWORD_PTR midi_data = encode_midi_data(0x09, note.channel, note.note, note.velocity);
	send_encoded_midi(midi_data);
}

void MidiWinMM::send_noteoff(const note_t& note)
{
	DWORD_PTR midi_data = encode_midi_data(0x08, note.channel, note.note, note.velocity);
	send_encoded_midi(midi_data);
}

void MidiWinMM::send_control(const control_t& control)
{
	DWORD_PTR midi_data = encode_midi_data(0x0B, control.channel, control.param, control.value);
	send_encoded_midi(midi_data);
}

void MidiWinMM::enqueue_midi(DWORD_PTR midi_data)
{
	std::unique_lock<std::mutex> lock(input_queue_mtx_);
	input_queue_.push_back(midi_data);
	input_queue_cv_.notify_one();
}

std::map<MidiWinMM::midi_id_t, midi_info_t> MidiWinMM::do_enumerate_input_devices()
{
	std::map<midi_id_t, midi_info_t> map;
	UINT num_devs = midiInGetNumDevs();
	MIDIINCAPS dev_caps;
	for (UINT i = 0; i < num_devs; ++i)
	{
		midiInGetDevCaps(i, &dev_caps, sizeof(MIDIINCAPS));
		midi_info_t info;
		info.name = dev_caps.szPname;
		map[i] = info;
	}
	return map;
}

std::map<MidiWinMM::midi_id_t, midi_info_t> MidiWinMM::do_enumerate_output_devices()
{
	std::map<midi_id_t, midi_info_t> map;
	UINT num_devs = midiOutGetNumDevs();
	MIDIOUTCAPS dev_caps;
	for (UINT i = 0; i < num_devs; ++i)
	{
		midiOutGetDevCaps(i, &dev_caps, sizeof(MIDIOUTCAPS));
		midi_info_t info;
		info.name = dev_caps.szPname;
		map[i] = info;
	}
	return map;
}

void MidiWinMM::run()
{
	std::deque<DWORD_PTR> temp_queue;
	while (!finish_)
	{
		while (!temp_queue.empty())
		{
			DWORD_PTR midi_data = temp_queue.front();
			temp_queue.pop_front();
			parse_midi_data(midi_data);
		}

		{
			std::unique_lock<std::mutex> lock(input_queue_mtx_);

			if (!input_queue_.empty())
			{
				// Copy out the current input queue so we can avoid deadlocks
				// by effectively calling another multimedia function from the
				// midi callback function (when sending midi).
				temp_queue.assign(input_queue_.begin(), input_queue_.end());
				input_queue_.clear();
				continue;
			}

			input_queue_cv_.wait_for(lock, std::chrono::milliseconds(5));
		}
	}
}

void MidiWinMM::parse_midi_data(DWORD_PTR midi_data)
{
	BYTE bytes[3];
	bytes[0] = static_cast<BYTE>(midi_data >> 16);
	bytes[1] = static_cast<BYTE>(midi_data >> 8);
	bytes[2] = static_cast<BYTE>(midi_data);

	BYTE type = bytes[2] >> 4;
	BYTE channel = bytes[2] & 0x0F;

	if (type == 0x0B) {
		control_t control;
		control.channel = channel;
		control.param = (bytes[1] & 0x7F);
		control.value = (bytes[0] & 0x7F);
		on_control(control);
	}
	else {
		note_t note;
		note.channel = channel;
		note.note = (bytes[1] & 0x7F);
		note.velocity = (bytes[0] & 0x7F);

		if (type == 0x09)
			on_noteon(note);
		else if (type == 0x08)
			on_noteoff(note);
	}
}

DWORD_PTR MidiWinMM::encode_midi_data(BYTE type, BYTE channel, BYTE param1, BYTE param2)
{
	DWORD_PTR data = param2;
	data <<= 8;
	data += param1;
	data <<= 8;
	data += (type << 4) + channel;
	return data;
}

void MidiWinMM::send_encoded_midi(DWORD_PTR midi_data)
{
	for (HMIDIOUT handle : open_outputs_)
	{
		midiOutShortMsg(handle, static_cast<DWORD>(midi_data));
	}
}


}
}

