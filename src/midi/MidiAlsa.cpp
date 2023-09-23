/**
 * @file 	MidiAlsa.cpp
 *
 * @date 	22.9.2023
 * @author 	Ondrej Slaby <slaby@iim.cz>
 * @copyright GNU Public License 3.0
 *
 */

#include "iimavlib/midi/MidiAlsa.h"
#include "iimavlib/Utils.h"
#include <alsa/seq.h>
#include <alsa/seq_event.h>
#include <alsa/seqmid.h>
#include <asm-generic/errno-base.h>
#include <chrono>
#include <functional>
#include <mutex>
#include <sstream>
#include <stdint.h>
#include <thread>

namespace iimavlib {

namespace midi {

MidiAlsa::MidiAlsa(const std::string &name)
{
    snd_seq_open(&seq_handle_, "default", SND_SEQ_OPEN_DUPLEX, SND_SEQ_NONBLOCK);
    snd_seq_set_client_name(seq_handle_, name.c_str());

    in_port_handle_ = snd_seq_create_simple_port(seq_handle_, "listen:in", 
        SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE, 
        SND_SEQ_PORT_TYPE_APPLICATION);
    out_port_handle_ = snd_seq_create_simple_port(seq_handle_, "play:out", 
        SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ, 
        SND_SEQ_PORT_TYPE_APPLICATION);
}

MidiAlsa::~MidiAlsa()
{
    snd_seq_close(seq_handle_);
}

void MidiAlsa::open_all_inputs()
{
    auto inputs = do_enumerate_input_devices();
    logger[log_level::debug] << "Opening all inputs: " << inputs.size();
    for (const auto& input : inputs)
    {
        open_input(input.first);
    }
}

void MidiAlsa::open_input(const midi_id_t& device)
{
    snd_seq_connect_from(seq_handle_, in_port_handle_, device.first, device.second);
}

void MidiAlsa::open_output(const midi_id_t& device)
{
    snd_seq_connect_to(seq_handle_, out_port_handle_, device.first, device.second);
}

void MidiAlsa::start()
{
    std::unique_lock<std::mutex> lock(thread_mutex_);
    if (thread_.joinable()) return;
    thread_ = std::thread(std::bind(&MidiAlsa::run, this));
    logger[log_level::debug] << "MIDI thread started";
    if (thread_.joinable()) return;
}

void MidiAlsa::stop()
{
    std::unique_lock<std::mutex> lock(thread_mutex_);
    if (!thread_.joinable()) return;
    finish_ = true;
    thread_.join();

    logger[log_level::debug] << "MIDI thread stopped.";
}

bool MidiAlsa::is_stopped() const
{
    return finish_;
}

void MidiAlsa::run()
{
    snd_seq_event_t *ev;
    int midi_remains;

    while (!finish_)
    {
        while ((midi_remains = snd_seq_event_input(seq_handle_, &ev)) > 0) {
            switch (ev->type) {
                case snd_seq_event_type::SND_SEQ_EVENT_NOTEON:
                    {
                        note_t note;
                        note.channel = ev->data.note.channel;
                        note.note = ev->data.note.note;
                        note.velocity = ev->data.note.velocity;
                        on_noteon(note);
                    } break;
                case snd_seq_event_type::SND_SEQ_EVENT_NOTEOFF:
                    {
                        note_t note;
                        note.channel = ev->data.note.channel;
                        note.note = ev->data.note.note;
                        note.velocity = ev->data.note.velocity;
                        on_noteoff(note);
                    } break;
                case snd_seq_event_type::SND_SEQ_EVENT_CONTROLLER:
                    {
                        control_t control;
                        control.channel = ev->data.control.channel;
                        control.param = ev->data.control.param;
                        control.value = ev->data.control.value;
                        on_control(control);
                    } break;
            }
        }

        if (midi_remains == -ENOSPC) {
            logger[log_level::info] << "Too many MIDI events. Some will be skipped.";
        }
        if (midi_remains < 0 && midi_remains != -EAGAIN) {
            logger[log_level::fatal] << "Error receiving midi. Bailing out.";
            finish_ = true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

snd_seq_event_t MidiAlsa::create_midi_event(snd_seq_event_type event_type)
{
    snd_seq_event_t ev;
    ev.flags = 
        (SND_SEQ_TIME_STAMP_REAL | SND_SEQ_PRIORITY_NORMAL);
    ev.queue = SND_SEQ_QUEUE_DIRECT;
    ev.source.port = out_port_handle_;
    ev.type = event_type;
    ev.dest.client = SND_SEQ_ADDRESS_SUBSCRIBERS;
    return ev;
}

void MidiAlsa::send_noteon(const note_t& note)
{
    snd_seq_event_t ev = create_midi_event(SND_SEQ_EVENT_NOTEON);
    ev.data.note.channel = note.channel;
    ev.data.note.note = note.note;
    ev.data.note.velocity = note.velocity;

    snd_seq_event_output_direct(seq_handle_, &ev);
}

void MidiAlsa::send_noteoff(const note_t& note)
{
    snd_seq_event_t ev = create_midi_event(SND_SEQ_EVENT_NOTEOFF);
    ev.data.note.channel = note.channel;
    ev.data.note.note = note.note;
    ev.data.note.off_velocity = note.velocity;

    snd_seq_event_output_direct(seq_handle_, &ev);
}

void MidiAlsa::send_control(const control_t& control)
{
    snd_seq_event_t ev = create_midi_event(SND_SEQ_EVENT_CONTROLLER);
    ev.data.control.channel = control.channel;
    ev.data.control.param = control.param;
    ev.data.control.value = control.value;

    snd_seq_event_output_direct(seq_handle_, &ev);
}

std::map<MidiAlsa::midi_id_t, midi_info_t> MidiAlsa::do_enumerate_input_devices()
{
    std::map<midi_id_t, midi_info_t> map;
    enumerate_midi_devices(map, SND_SEQ_PORT_CAP_WRITE, SND_SEQ_PORT_TYPE_MIDI_GENERIC);
    return map;
}

std::map<MidiAlsa::midi_id_t, midi_info_t> MidiAlsa::do_enumerate_output_devices()
{
    std::map<midi_id_t, midi_info_t> map;
    enumerate_midi_devices(map, SND_SEQ_PORT_CAP_READ, SND_SEQ_PORT_TYPE_MIDI_GENERIC);
    return map;
}

void MidiAlsa::enumerate_midi_devices(std::map<midi_id_t, midi_info_t>& map, int capabilities, int port_type)
{
    snd_seq_t* handle;
    snd_seq_open(&handle, "default", SND_SEQ_OPEN_DUPLEX, 0);

    snd_seq_client_info_t *info;
    snd_seq_client_info_alloca(&info);
    snd_seq_client_info_set_client(info, -1);

    while (snd_seq_query_next_client(handle, info) == 0)
    {
        int client = snd_seq_client_info_get_client(info);

        std::string client_name(snd_seq_client_info_get_name(info));

        snd_seq_port_info_t *port_info;
        snd_seq_port_info_alloca(&port_info);
        snd_seq_port_info_set_port(port_info, -1);
        snd_seq_port_info_set_client(port_info, client);

        while (snd_seq_query_next_port(handle, port_info) == 0)
        {
            if (!(snd_seq_port_info_get_capability(port_info) & capabilities))
                continue;
            if (!(snd_seq_port_info_get_type(port_info) & port_type))
                continue;

            std::stringstream ss;
            ss << client_name << ":" << snd_seq_port_info_get_name(port_info);

            midi_info_t minfo;
            minfo.name = ss.str();

            midi_id_t mid = {client, snd_seq_port_info_get_port(port_info)};
            map[mid] = minfo;
        }
    }

    snd_seq_close(handle);
}

}

}