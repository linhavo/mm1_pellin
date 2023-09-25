/**
 * @file 	MidiPlatform.h
 *
 * @date 	22.9.2023
 * @author 	Ondrej Slaby <slaby@iim.cz>
 * @copyright GNU Public License 3.0
 *
 */

#ifndef MIDIPLATFORM_H_
#define MIDIPLATFORM_H_

#include "../PlatformDefs.h"
#ifdef _WIN32
#include "MidiWinMM.h"
namespace iimavlib {
namespace midi {
typedef iimavlib::midi::MidiWinMM MidiPlatformDevice;
}
}
#else
#ifdef __linux__
#include "MidiAlsa.h"
namespace iimavlib {
namespace midi {
typedef iimavlib::midi::MidiAlsa MidiPlatformDevice;
}
}
#else
#error Unsupported platform
#endif
#endif

#endif //MIDIPLATFORM_H_