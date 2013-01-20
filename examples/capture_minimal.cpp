/*
 * capture_test.cpp
 *
 *  Created on: 19.1.2013
 *      Author: neneko
 */

#include "iimaudio.h"
#include <vector>

using namespace iimaudio;
int main()
{
	CaptureDevice device;
	std::vector<int16_t> buffer(16384,0);
	size_t captured;
	return_type_t error;

	device.start_capture();
	captured = device.capture_data(buffer,error);
	/* Process data in buffer
	...
	*/
}

