/*
 * AudioTypes.h
 *
 *  Created on: 19.1.2013
 *      Author: neneko
 */

#ifndef AUDIOTYPES_H_
#define AUDIOTYPES_H_
#include <cstdint>
#include <vector>
#include <string>

namespace iimaudio {
enum class sampling_rate_t: uint8_t {
	rate_unknown,
	rate_8kHz,
	rate_11kHz,
	rate_22kHz,
	rate_44kHz,
	rate_48kHz
};

enum class sampling_format_t: uint8_t {
	format_unknown,
	format_8bit_unsigned,
	format_8bit_signed,
	format_16bit_unsigned,
	format_16bit_signed,
};


enum class action_type_t: uint8_t {
	action_capture,
	action_playback
};

enum class return_type_t: uint8_t {
	ok,
	failed,
	xrun,
	invalid,
	buffer_full,
	buffer_empty,
	busy
};

uint32_t convert_rate_to_int(const sampling_rate_t rate);
sampling_rate_t convert_int_to_rate(const size_t irate);
uint32_t get_sample_size(const sampling_format_t format);
std::string error_string(const return_type_t error);

std::string sampling_rate_string(const sampling_rate_t rate);
std::string sampling_format_string(const sampling_format_t format);


struct audio_buffer_t {
	std::vector<uint8_t> data;
	bool empty = true;
	uint32_t position = 0;
};

struct audio_params_t {
	sampling_rate_t rate;
	sampling_format_t format;
	uint8_t num_channels;
	audio_params_t(sampling_rate_t rate = sampling_rate_t::rate_22kHz, sampling_format_t format=sampling_format_t::format_16bit_signed, uint8_t num_channels=1):
		rate(rate),format(format),num_channels(num_channels) {}
	uint16_t sample_size() const { return get_sample_size(format)*num_channels; }
};




}
#endif /* AUDIOTYPES_H_ */
