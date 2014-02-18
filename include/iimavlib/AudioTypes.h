/**
 * @file 	AudioTypes.h
 *
 * @date 	19.1.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 * This file defines basic types used in the library
 */

#ifndef AUDIOTYPES_H_
#define AUDIOTYPES_H_
#include "PlatformDefs.h"
#include <cstdint>
#include <vector>
#include <string>
#include <set>
#include <stdexcept>
#include <type_traits>
#include "AudioSample.h"
namespace iimavlib {

constexpr int number_of_channels = 2;
typedef int16_t sample_format_t;


/*!
 * @brief Sample rates usable in the library
 */
enum class sampling_rate_t: uint8_t {
	rate_unknown,//!< rate_unknown
	rate_8kHz,   //!< rate_8kHz
	rate_11kHz,  //!< rate_11kHz
	rate_22kHz,  //!< rate_22kHz
	rate_44kHz,  //!< rate_44kHz
	rate_48kHz,  //!< rate_48kHz
	rate_96kHz,  //!< rate_96kHz
	rate_192kHz  //!< rate_192kHz
};


/*!
 * @brief Actions defined for the devices
 */
enum class action_type_t: uint8_t {
	action_capture,//!< action_capture
	action_playback//!< action_playback
};

/*!
 * @brief Error codes
 */
enum class error_type_t: uint8_t {
	ok,          //!< No error
	failed,      //!< Generic error
	xrun,        //!< Overrun or underrun occured
	invalid,     //!< Device is in an invalid state
	buffer_full, //!< Buffer is full (and nothing couldn't be added at the moment)
	buffer_empty,//!< Buffer in empty, so no data couldn't be read
	busy,         //!< Device is busy, try again later
	unsupported	 //!< Operation of format is not supported
};

/*!
 * @brief Converts @em sampling_rate_t to an integer value
 *
 * @param rate Sampling rate to convert
 * @return Integer representation of the sampling rate
 */

EXPORT uint32_t convert_rate_to_int(const sampling_rate_t rate);
EXPORT sampling_rate_t convert_int_to_rate(const uint32_t irate);

/*!
 * @brief Converts an error code to a corresponding string value
 * @param error Error code to convert
 * @return Human readable representation of the error code
 */
EXPORT std::string error_string(const error_type_t error);
EXPORT std::string sampling_rate_string(const sampling_rate_t rate);


struct audio_params_t {
	sampling_rate_t rate;
	bool enable_resampling;

	audio_params_t(sampling_rate_t rate = sampling_rate_t::rate_44kHz):
		rate(rate),enable_resampling(true) {}
	uint16_t sample_size() const { return sizeof(int16_t)*2; }

	audio_params_t(uint32_t rate_num):enable_resampling(true) {
		rate = convert_int_to_rate(rate_num);
		if (rate == sampling_rate_t::rate_unknown)
			throw std::runtime_error("Unsupported sampling rate provided");
	}
};

struct audio_info_t {
	std::string name;
	std::set<sampling_rate_t> supported_rates;
	bool default_;
	audio_info_t():/*max_channels(0),*/default_(false) {}
};



struct audio_buffer_t {

	std::vector<audio_sample_t> data;

	audio_params_t params;
	std::size_t valid_samples;

// private fields
	bool empty;
	uint32_t position;
	audio_buffer_t():valid_samples(0),empty(true),position(0) {}
};

template<typename T>
struct circular_buffer_t {
	std::vector<T> data;
	std::size_t start_;
	std::size_t end_;
	circular_buffer_t(std::size_t size):data(size,0),start_(0),end_(0) {}
	bool store_data(const T *in_data, std::size_t size) {
		bool overflow = false;
		const std::size_t till_end = data.size() - end_;
		const std::size_t copy1 = std::min(till_end,size);
		std::copy(in_data,in_data+copy1,data.begin()+end_);
		if ((end_ < start_) && (end_ +copy1 >= start_ )) {
			overflow = true;
			start_ = (end_+copy1+1)%data.size();
		}
		end_=(end_+copy1)%data.size();
		const std::size_t remaining = size - copy1;
		if (!remaining) return overflow;

		return store_data(in_data+copy1,remaining);
	}
	std::size_t get_data_block(T* out_data, std::size_t max_size) {
		if (start_ < end_) {
			const std::size_t avail = std::min(end_ - start_,max_size);
			std::copy(data.begin()+start_,data.begin()+start_+avail,out_data);
			start_ = (start_ + avail)%data.size();
			return avail;
		}
		if (start_ > end_) {
			const std::size_t till_end = data.size() - start_;
			const std::size_t copy1 = std::min(max_size, till_end);
			std::copy(data.begin()+start_,data.begin()+start_+copy1,out_data);
			start_ = (start_ + copy1)%data.size();
			if (copy1 >= max_size) return copy1;
			return get_data_block(out_data+copy1,max_size-copy1);
		}
		return 0;
	}
};



}
#endif /* AUDIOTYPES_H_ */
