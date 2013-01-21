/**
 * @file 	capture_minimal.cpp
 *
 * @date 	20.1.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 * Simplest example of audio capture.
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
	device.capture_data(buffer,error);
	/* Process data in buffer
	...
	*/
}

