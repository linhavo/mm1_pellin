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
#include "MidiTypes.h"
#include "MidiPlatform.h"
#include <string>
#include <map>

namespace iimavlib {

namespace midi {

template<class Device = MidiPlatformDevice>
class MidiDevice : private Device {
public:
    // Opaque, platform specific MIDI device ID type
    typedef typename Device::midi_id_t midi_id_t;

    /*!
     * @brief Default constructor
     *
     * Creates the MIDI device, specifying its name if applicable.
     *
     * @param name Name of this application's midi device (ignored in Windows)
    */
    MidiDevice(const std::string& name = "iimavlib") : Device(name) {};
    virtual ~MidiDevice() {};

    /*!
     * @brief Open all available input devices for event consumption
     *
     * Opens each input device available on the system (both software and hardware) that
     * can supply basic MIDI events (noteon, noteoff, control). Events can be received
     * immediately after this function returns.
    */
    void open_all_inputs() override;
    /*!
     * @brief Open the specified input device for event cunsumption
     *
     * Opens the specified input device to consume basic MIDI events (noteon, noteoff, control).
     * Events can be received immediately after this function returns.
     *
     * @param device Which device to open
    */
    void open_input(const midi_id_t& device);
    /*!
     * @brief Open the specified output device to send events to
     *
     * Opens the specified output device, making it one of the consumers of this application's
     * MIDI events. Each event sent afterwards will be received by the newly opened device.
     *
     * @param device Which device to open
    */
    void open_output(const midi_id_t& device);

    /*!
     * @brief Start receiving MIDI events
     *
     * Starts a thread which collects received MIDI events and passes them on to virtual functions
     * @em on_noteon, @em on_noteoff and @em on_control. Derived classes should override these functions
     * to receive the appropriate events.
    */
    void start() override;
    /*!
     * @brief Stop receiving MIDI events
     *
     * Stops the MIDI event collection thread. No new events will be received after calling this function.
    */
    void stop() override;
    /*!
     * @brief Whether the MIDI event collection thread is stopped
     * @return Returns status of the thread
    */
    bool is_stopped() const override;

    /*!
     * @brief Send a MIDI noteon event to connected outputs
     * @param note Note parameters describing the event to send
    */
    void send_noteon(const note_t& note) override;
    /*!
     * @brief Send a MIDI noteoff event to connected outputs
     * @param note Note parameters describing the event to send
    */
    void send_noteoff(const note_t& note) override;
    /*!
     * @brief Send a MIDI control event to connected outputs
     * @param control Control parameters describing the event to send
    */
    void send_control(const control_t& control) override;

    /*!
     * @brief Get information about available input devices
     *
     * Collects IDs paired with information about all available input
     * devices on the system. These IDs can be used in @em open_input
     * to open the specified device and receive MIDI events from it.
     *
     * @return Returns a map of IDs paired to info structures
    */
    static std::map<midi_id_t, midi_info_t> do_enumerate_input_devices();
    /*!
     * @brief Get information about available output devices
     *
     * Collects IDs paired with information about output devices available
     * on the system. The returned IDs can be used in @em open_output to open
     * the specified device to send events to.
     * 
     * @return Returns a map of IDs paired to info structures
    */
    static std::map<midi_id_t, midi_info_t> do_enumerate_output_devices();

private:
    /*!
     * @brief MIDI noteon event callback function
     *
     * Derived classes should override this function to receive noteon events from
     * connected inputs.
    */
    virtual void on_noteon(const note_t& note) override;
    /*!
     * @brief MIDI noteoff event callback function
     *
     * Derived classes should override this function to receive noteoff events from
     * connected inputs.
    */
    virtual void on_noteoff(const note_t& note) override;
    /*!
     * @brief MIDI control event callback function
     *
     * Derived classes should override this function to receive control events from
     * connected inputs.
    */
    virtual void on_control(const control_t& control) override;
};

// Convenience alias to hide type parameter brackets
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