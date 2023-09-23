/**
 * @file 	enumerate_devices.cpp
 *
 * @date 	22.9.2023
 * @author 	Ondrej Slaby <slaby@iim.cz>
 * @copyright GNU Public License 3.0
 *
 * Example enumerating all midi devices
 */

#include "iimavlib.h"
#include "iimavlib/Utils.h"
#include "iimavlib/midi/MidiDevice.h"

using namespace iimavlib;

int main()
{
	logger[log_level::info] << "Input devices:";
	for (auto dev: midi::Midi::do_enumerate_input_devices()) 
		logger[log_level::info] << "Device: '" << dev.first.first << "," << dev.first.second << "': " << dev.second.name;
	logger[log_level::info] << "Playback devices:";
	for (auto dev: midi::Midi::do_enumerate_output_devices())
		logger[log_level::info] << "Device: '" << dev.first.first << "," << dev.first.second << "': " << dev.second.name;
}

