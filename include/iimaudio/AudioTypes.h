/*
 * AudioTypes.h
 *
 *  Created on: 19.1.2013
 *      Author: neneko
 */

#ifndef AUDIOTYPES_H_
#define AUDIOTYPES_H_
#include <cstdint>

namespace iimaudio {
enum class sampling_rate_t: uint8_t {
	rate_8kHz,
	rate_11kHz,
	rate_22kHz,
	rate_44kHz,
	rate_48kHz
};

enum class sampling_format_t: uint8_t {
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
	xrun
};

struct audio_params_t {
	sampling_rate_t rate;
	sampling_format_t format;
	uint8_t num_channels;
	audio_params_t(sampling_rate_t rate = sampling_rate_t::rate_22kHz, sampling_format_t format=sampling_format_t::format_16bit_signed, uint8_t num_channels=1):
		rate(rate),format(format),num_channels(num_channels) {}
};

}
#endif /* AUDIOTYPES_H_ */
