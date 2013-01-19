/*
 * Utils.cpp
 *
 *  Created on: 19.1.2013
 *      Author: neneko
 */
#include "iimaudio/Utils.h"

namespace iimaudio {
Log log(std::cerr);

log_level Log::mode =
#ifdef NDEBUG
		log_level::info;
#else
		log_level::debug;
#endif

namespace {
template<typename first, typename second, typename val>
second get_from_map(const std::map<first,second> &map_, const first& key_, const val &default_)
{
	auto it = map_.find(key_);
	if (it == map_.end()) return default_;
	return it->second;
}
}

namespace {
std::map<sampling_rate_t, size_t> rates_to_int = InitMap<sampling_rate_t, size_t>
(sampling_rate_t::rate_8kHz,	8000)
(sampling_rate_t::rate_11kHz, 	11025)
(sampling_rate_t::rate_22kHz, 	22050)
(sampling_rate_t::rate_44kHz, 	44100)
(sampling_rate_t::rate_48kHz, 	48000);
}

size_t convert_rate_to_int(const sampling_rate_t rate) {
	return get_from_map(rates_to_int, rate, 0);
}

namespace {
std::map<sampling_format_t, size_t> sample_sizes = InitMap<sampling_format_t, size_t>
(sampling_format_t::format_8bit_unsigned,	1)
(sampling_format_t::format_8bit_signed,		1)
(sampling_format_t::format_16bit_unsigned,	2)
(sampling_format_t::format_16bit_signed,	2);
}



size_t get_sample_size(const sampling_format_t format)
{
	return get_from_map(sample_sizes, format, 0);
}
namespace {
std::map<return_type_t, std::string> error_strings = InitMap<return_type_t, std::string>
(return_type_t::ok, 		"OK")
(return_type_t::failed, 	"Failed")
(return_type_t::xrun, 		"Under/Overrun occurred");
}

std::string get_error_string(const return_type_t error)
{
	return get_from_map(error_strings, error, "Unknown error");
}
}



