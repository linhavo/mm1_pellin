/**
 * @file 	AudioTypes.cpp
 *
 * @date 	19.1.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 */

#include "iimaudio/Utils.h"
#include <algorithm>
#include <map>
#include <string>
namespace iimaudio {

namespace {
template<typename first, typename second, typename val>
second get_from_map(const std::map<first,second> &map_, const first& key_, const val &default_)
{
	auto it = map_.find(key_);
	if (it == map_.end()) return default_;
	return it->second;
}
template<typename first, typename second, typename val>
first get_from_map_reverse(const std::map<first,second> &map_, const second& key_, const val &default_)
{
	auto it = std::find_if(map_.begin(),map_.end(),[key_](const std::pair<first,second>& v){return v.second == key_;});
	if (it == map_.end()) return default_;
	return it->first;
}

}

namespace {
std::map<sampling_rate_t, size_t> rates_to_int = InitMap<sampling_rate_t, size_t>
(sampling_rate_t::rate_8kHz,	8000)
(sampling_rate_t::rate_11kHz, 	11025)
(sampling_rate_t::rate_22kHz, 	22050)
(sampling_rate_t::rate_44kHz, 	44100)
(sampling_rate_t::rate_48kHz, 	48000)
(sampling_rate_t::rate_96kHz, 	96000)
(sampling_rate_t::rate_192kHz, 192000);
}

uint32_t convert_rate_to_int(const sampling_rate_t rate) {
	return get_from_map(rates_to_int, rate, 0);
}
sampling_rate_t convert_int_to_rate(const size_t irate)
{
	return get_from_map_reverse(rates_to_int, irate, sampling_rate_t::rate_unknown);
}
namespace {
std::map<sampling_format_t, size_t> sample_sizes = InitMap<sampling_format_t, size_t>
(sampling_format_t::format_8bit_unsigned,	1)
(sampling_format_t::format_8bit_signed,		1)
(sampling_format_t::format_16bit_unsigned,	2)
(sampling_format_t::format_16bit_signed,	2);
}



uint32_t get_sample_size(const sampling_format_t format)
{
	return get_from_map(sample_sizes, format, 0);
}

namespace {
std::map<sampling_rate_t, std::string> sampling_rate_strings = InitMap<sampling_rate_t, std::string>
(sampling_rate_t::rate_unknown,		"Unknown sampling rate")
(sampling_rate_t::rate_8kHz,		"8.000 Hz")
(sampling_rate_t::rate_11kHz,		"11.025 Hz")
(sampling_rate_t::rate_22kHz,		"22.050 Hz")
(sampling_rate_t::rate_44kHz,		"44.100 Hz")
(sampling_rate_t::rate_48kHz,		"48.000 Hz");

}
std::string sampling_rate_string(const sampling_rate_t rate)
{
	return get_from_map(sampling_rate_strings, rate, "Unknown");
}
namespace {
std::map<sampling_format_t, std::string> sampling_format_strings = InitMap<sampling_format_t, std::string>
(sampling_format_t::format_unknown, "Unknown format")
(sampling_format_t::format_8bit_signed, "8bit, signed")
(sampling_format_t::format_8bit_unsigned, "8bit, unsigned")
(sampling_format_t::format_16bit_signed, "16bit, signed")
(sampling_format_t::format_16bit_unsigned, "16bit, unsigned");
}

std::string sampling_format_string(const sampling_format_t format)
{
	return get_from_map(sampling_format_strings, format, "Unknown");
}

namespace {
std::map<return_type_t, std::string> error_strings = InitMap<return_type_t, std::string>
(return_type_t::ok, 		"OK")
(return_type_t::failed, 	"Failed")
(return_type_t::xrun, 		"Under/Overrun occurred");
}

std::string error_string(const return_type_t error)
{
	return get_from_map(error_strings, error, "Unknown error");
}

}
