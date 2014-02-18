/**
 * @file 	AudioTypes.cpp
 *
 * @date 	19.1.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 */

#include "iimavlib/Utils.h"
#include <algorithm>
#include <map>
#include <string>
namespace iimavlib {

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
std::map<sampling_rate_t, uint32_t> rates_to_int = {//InitMap<sampling_rate_t, size_t>
{sampling_rate_t::rate_8kHz,	8000},
{sampling_rate_t::rate_11kHz, 	11025},
{sampling_rate_t::rate_22kHz, 	22050},
{sampling_rate_t::rate_44kHz, 	44100},
{sampling_rate_t::rate_48kHz, 	48000},
{sampling_rate_t::rate_96kHz, 	96000},
{sampling_rate_t::rate_192kHz, 192000}
};

}

uint32_t convert_rate_to_int(const sampling_rate_t rate) {
	return static_cast<uint32_t>(get_from_map(rates_to_int, rate, 0));
}
sampling_rate_t convert_int_to_rate(const uint32_t irate)
{
	return get_from_map_reverse(rates_to_int, irate, sampling_rate_t::rate_unknown);
}

namespace {
std::map<sampling_rate_t, std::string> sampling_rate_strings = {
{sampling_rate_t::rate_unknown,		"Unknown sampling rate"},
{sampling_rate_t::rate_8kHz,		"8.000 Hz"},
{sampling_rate_t::rate_11kHz,		"11.025 Hz"},
{sampling_rate_t::rate_22kHz,		"22.050 Hz"},
{sampling_rate_t::rate_44kHz,		"44.100 Hz"},
{sampling_rate_t::rate_48kHz,		"48.000 Hz"},
{sampling_rate_t::rate_96kHz,		"96.000 Hz"},
{sampling_rate_t::rate_192kHz,		"192.000 Hz"}};

}
std::string sampling_rate_string(const sampling_rate_t rate)
{
	return get_from_map(sampling_rate_strings, rate, "Unknown");
}

namespace {
std::map<error_type_t, std::string> error_strings = {//InitMap<error_type_t, std::string>
{error_type_t::ok, 		"OK"},
{error_type_t::failed, 	"Failed"},
{error_type_t::xrun, 		"Under/Overrun occurred"}};

}

std::string error_string(const error_type_t error)
{
	return get_from_map(error_strings, error, "Unknown error");
}

}
