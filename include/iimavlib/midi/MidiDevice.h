/**
 * @file 	MidiDevice.h
 *
 * @date 	22.9.2023
 * @author 	Ondrej Slaby <slaby@iim.cz>
 * @copyright GNU Public License 3.0
 *
 */

#ifndef MIDIDEVICE_H_
#define MIDIDEVICE_H_

#include "../PlatformDefs.h"
#include "iimavlib/midi/MidiAlsa.h"
#include "iimavlib/midi/MidiTypes.h"
#include "iimavlib/midi/MidiPlatform.h"
#include <string>
#include <map>

namespace iimavlib {

namespace midi {

template<class Device = MidiPlatformDevice>
class EXPORT MidiDevice : private Device {
public:
    typedef typename MidiAlsa::midi_id_t midi_id_t;

    MidiDevice(const std::string& name) : Device(name) {};
    virtual ~MidiDevice() {};

    void open_all_inputs() override;
    void open_input(const midi_id_t& device);
    void open_output(const midi_id_t& device);

    void start() override;
    void stop() override;
    bool is_stopped() const override;

    void send_noteon(const note_t& note) override;
    void send_noteoff(const note_t& note) override;
    void send_control(const control_t& control) override;

    static std::map<midi_id_t, midi_info_t> do_enumerate_input_devices();
    static std::map<midi_id_t, midi_info_t> do_enumerate_output_devices();

private:
    virtual void on_noteon(const note_t& note) override;
    virtual void on_noteoff(const note_t& note) override;
    virtual void on_control(const control_t& control) override;
};

typedef MidiDevice<> Midi;


template<class Device>
std::map<typename MidiDevice<Device>::midi_id_t, midi_info_t> MidiDevice<Device>::do_enumerate_input_devices()
{
    return Device::do_enumerate_input_devices();
}

template<class Device>
std::map<typename MidiDevice<Device>::midi_id_t, midi_info_t> MidiDevice<Device>::do_enumerate_output_devices()
{
    return Device::do_enumerate_output_devices();
}

template<class Device>
void MidiDevice<Device>::open_all_inputs()
{
    Device::open_all_inputs();
}

template<class Device>
void MidiDevice<Device>::open_input(const typename MidiDevice<Device>::midi_id_t& device)
{
    Device::open_input(device);
}

template<class Device>
void MidiDevice<Device>::open_output(const typename MidiDevice<Device>::midi_id_t& device)
{
    Device::open_output(device);
}

template<class Device>
void MidiDevice<Device>::start()
{
    Device::start();
}

template<class Device>
void MidiDevice<Device>::stop()
{
    Device::stop();
}

template<class Device>
bool MidiDevice<Device>::is_stopped() const
{
    return Device::is_stopped();
}

template<class Device>
void MidiDevice<Device>::send_noteon(const note_t& note)
{
    Device::send_noteon(note);
}

template<class Device>
void MidiDevice<Device>::send_noteoff(const note_t& note)
{
    Device::send_noteoff(note);
}

template<class Device>
void MidiDevice<Device>::send_control(const control_t& control)
{
    Device::send_control(control);
}

template<class Device>
void MidiDevice<Device>::on_noteon(const note_t&)
{
}

template<class Device>
void MidiDevice<Device>::on_noteoff(const note_t&)
{
}

template<class Device>
void MidiDevice<Device>::on_control(const control_t&)
{
}

}

}



#endif