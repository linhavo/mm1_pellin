/**
 * @file 	playback_sine.cpp
 *
 * @date 	20.1.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 */

#include "iimavlib.h"
#include "iimavlib/Utils.h"
#include <vector>
#include <cmath>
#include <algorithm>
#include <limits>
using namespace iimavlib;
const uint16_t uint16_max = std::numeric_limits<uint16_t>::max();
const double pi = std::atan(1)*4;
template<typename T>
void synthesize(double& t, double frequency, std::size_t rate, std::vector<T>& data)
{
	std::for_each(data.begin(),data.end(),[&t,frequency,rate](T& v)//{v=1;});
			{v=static_cast<uint16_t>(uint16_max*std::sin(t));t=t+2.0*pi*frequency/rate;if (t>2*pi) t=t-2*pi;});
}


int main()
{
	audio_params_t 		params(sampling_rate_t::rate_44kHz,
							sampling_format_t::format_16bit_signed,
							1);
	PlaybackDevice 		device(params);
	const std::size_t	buffer_size 	= 512;
	const std::size_t	buffer_count 	= 4;
	const double	 	frequency		= 1000.0;
	const std::size_t rate = convert_rate_to_int(params.rate);
	double time = 0.0;
	std::vector<int16_t> data(buffer_size);

	device.set_buffers(buffer_count,buffer_size);

	for (size_t i=0;i<buffer_count;++i) {
		synthesize(time,frequency,rate,data);
		device.fill_buffer(data);
	}
	synthesize(time,frequency,rate,data);
	device.start_playback();
	while (true) {
		error_type_t ret = device.update();
		if (ret == error_type_t::busy) continue;
		if (ret != error_type_t::ok) {
			logger[log_level::fatal] << "Failed to update";
			break;
		}
		ret = device.fill_buffer(data);
		if (ret == error_type_t::buffer_full) continue;
		if (ret != error_type_t::ok) break;
		synthesize(time,frequency,rate,data);
	}
}



