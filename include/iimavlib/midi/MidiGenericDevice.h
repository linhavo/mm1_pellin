/**
 * @file 	MidiGenericDevice.h
 *
 * @date 	22.9.2023
 * @author 	Ondrej Slaby <slaby@iim.cz>
 * @copyright GNU Public License 3.0
 *
 */

#ifndef MIDIGENERICDEVIDE_H_
#define MIDIGENERICDEVIDE_H_

#include "../PlatformDefs.h"
#include "MidiTypes.h"

namespace iimavlib {

namespace midi {

class EXPORT MidiGenericDevice {
public:
#ifdef MODERN_COMPILER
    MidiGenericDevice() = default;
    virtual ~MidiGenericDevice() = default;
#else
    virtual ~MidiMidiGenericDevice() {};
#endif

    virtual void open_all_inputs() = 0;

    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool is_stopped() const = 0;

    virtual void send_noteon(const note_t& note) = 0;
    virtual void send_noteoff(const note_t& note) = 0;
    virtual void send_control(const control_t& control) = 0;
};

}

}


#endif //MIDIGENERICDEVICE_H_