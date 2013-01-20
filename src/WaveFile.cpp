/*
 * WaveFile.cpp
 *
 *  Created on: 19.1.2013
 *      Author: neneko
 */

#include "iimaudio/WaveFile.h"
#include "iimaudio/Utils.h"
#include <stdexcept>
namespace iimaudio {

WaveFile::WaveFile(const std::string& filename, audio_params_t params)
:params_(params)
{
	if ((params_.format != sampling_format_t::format_8bit_unsigned) &&
			(params_.format != sampling_format_t::format_16bit_signed)) {
		throw std::runtime_error("Only 8bit unsigned and 16bit signed formats are supported!");
	}
	file_.open(filename,std::ios::binary | std::ios::out | std::ios::trunc);
	if (!file_.is_open()) throw std::runtime_error("Failed to open the output file");
	header_ = wav_header_t(params_.num_channels,
								convert_rate_to_int(params_.rate),
								get_sample_size(params_.format)*8);
	update(0);
}

WaveFile::WaveFile(const std::string& filename)
{
	file_.open(filename,std::ios::binary | std::ios::in);
	if (!file_.is_open()) throw std::runtime_error("Failed to open the input file");
	file_.read(reinterpret_cast<char*>(&header_),sizeof(wav_header_t));
	if (file_.gcount()!=sizeof(wav_header_t))
		throw std::runtime_error("Failed to read wav header");
	params_.rate = convert_int_to_rate(header_.rate);
	switch (header_.bps) {
		case 8: params_.format = sampling_format_t::format_8bit_unsigned;
			break;
		case 16: params_.format = sampling_format_t::format_16bit_signed;
			break;
		default:
			throw std::runtime_error("Unsupported input format");
	}
	params_.num_channels = header_.channels;
}

void WaveFile::update(size_t new_data_size)
{
	header_.add_size(new_data_size);
	file_.seekp(0,std::ios::beg);
	file_.write(reinterpret_cast<char*>(&header_),sizeof(header_));
	file_.seekp(0,std::ios::end);
}

audio_params_t WaveFile::get_params() const
{
	return params_;
}
}

