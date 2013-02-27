/**
 * @file 	enumerate_devices.cpp
 *
 * @date 	20.1.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 * Example enumerating all capture and playback devices
 */

#include "iimavlib.h"
#include "iimavlib/Utils.h"

using namespace iimavlib;
int main()
{
	logger[log_level::info] << "Capture devices:";
	for (auto dev: CaptureDevice::enumerate_devices()) 
		logger[log_level::info] << (dev.second.default_?"*":" ") << "Device: '"<< dev.first << "': " << dev.second.name;
	logger[log_level::info] << "Playback devices:";
	for (auto dev: PlaybackDevice::enumerate_devices())
		logger[log_level::info] << (dev.second.default_?"*":" ") << "Device: '"<< dev.first << "': " << dev.second.name;
}

