/**
 * @file 	MidiAlsa.h
 *
 * @date 	22.9.2023
 * @author 	Ondrej Slaby <slaby@iim.cz>
 * @copyright GNU Public License 3.0
 *
 */

#ifndef MIDIALSA_H_
#define MIDIALSA_H_

#include "../PlatformDefs.h"
#include "MidiGenericDevice.h"
#include <alsa/seq_event.h>
#include <atomic>
#include <map>
#include <mutex>
#include <ostream>
#include <string>
#include <alsa/asoundlib.h>
#include <thread>

namespace iimavlib {

namespace midi {

class MidiAlsa : public MidiGenericDevice {
public:
    typedef std::pair<int, int> midi_id_t;

    MidiAlsa(const std::string& name);
    virtual ~MidiAlsa();

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
    snd_seq_t *seq_handle_;
    int in_port_handle_, out_port_handle_;

    std::mutex thread_mutex_;
    std::thread thread_;
    std::atomic<bool> finish_;

    void run();
    snd_seq_event_t create_midi_event(snd_seq_event_type event_type);

    virtual void on_noteon(const note_t& note) = 0;
    virtual void on_noteoff(const note_t& note) = 0;
    virtual void on_control(const control_t& control) = 0;

    static void enumerate_midi_devices(std::map<midi_id_t, midi_info_t>& map, int capabilities, int port_type);
};

}

}

std::ostream& operator<<(std::ostream& stream, const iimavlib::midi::MidiAlsa::midi_id_t& value);

#endif