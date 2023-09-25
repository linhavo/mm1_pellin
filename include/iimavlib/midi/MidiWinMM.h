#ifndef MIDIWINMM_H_
#define MIDIWINMM_H_

#include "../PlatformDefs.h"
#include "MidiGenericDevice.h"
#include <windows.h>
#include <mmeapi.h>
#include <map>
#include <thread>
#include <deque>
#include <mutex>
#include <set>

namespace iimavlib
{

namespace midi
{

class EXPORT MidiWinMM : public MidiGenericDevice
{
public:
    typedef UINT midi_id_t;

    MidiWinMM(const std::string& name);
    virtual ~MidiWinMM();

    void open_all_inputs() override;
    void open_input(const midi_id_t& device);
    void open_output(const midi_id_t& device);

    void start() override;
    void stop() override;
    bool is_stopped() const override;

    void send_noteon(const note_t& note) override;
    void send_noteoff(const note_t& note) override;
    void send_control(const control_t& control) override;

    void enqueue_midi(DWORD_PTR midi_data);

    static std::map<midi_id_t, midi_info_t> do_enumerate_input_devices();
    static std::map<midi_id_t, midi_info_t> do_enumerate_output_devices();

private:
    std::atomic<bool> finish_;
    std::deque<DWORD_PTR> input_queue_;
    std::mutex input_queue_mtx_;
    std::condition_variable input_queue_cv_;
    std::thread thread_;
    std::mutex thread_mutex_;
    std::set<HMIDIIN> open_inputs_;
    std::set<HMIDIOUT> open_outputs_;

    void run();
    void parse_midi_data(DWORD_PTR midi_data);
    DWORD_PTR encode_midi_data(BYTE type, BYTE channel, BYTE param1, BYTE param2);
    void send_encoded_midi(DWORD_PTR midi_data);

    virtual void on_noteon(const note_t& note) = 0;
    virtual void on_noteoff(const note_t& note) = 0;
    virtual void on_control(const control_t& control) = 0;
};

}

}

#endif //MIDIWINMM_H_