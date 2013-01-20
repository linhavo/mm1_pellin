/*
 * capture_test.cpp
 *
 *  Created on: 19.1.2013
 *      Author: neneko
 */

#include "iimaudio.h"
#include "iimaudio/Utils.h"

using namespace iimaudio;
int main()
{
	for (auto dev: CaptureDevice::enumerate_devices()) 
		logger[log_level::info] << (dev.second.default?"*":" ") << "Device: '"<< dev.first << "': " << dev.second.name << "\n";
}

