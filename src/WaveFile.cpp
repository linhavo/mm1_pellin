/**
 * @file 	WaveFile.cpp
 *
 * @date 	19.1.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 */

#include "iimavlib/WaveFile.h"
#include "iimavlib/Utils.h"
#include <stdexcept>
namespace iimavlib {

WaveFile::WaveFile(const std::string& filename, audio_params_t params)
:params_(params),mono_source_(false)
{
	file_.open(filename,std::ios::binary | std::ios::out | std::ios::trunc);
	if (!file_.is_open()) throw std::runtime_error("Failed to open the output file");
	header_ = wav_header_t(number_of_channels,
								convert_rate_to_int(params_.rate),
								16);//params_.sample_size()*8);
	update(0);
}

WaveFile::WaveFile(const std::string& filename):mono_source_(false)
{
	file_.open(filename,std::ios::binary | std::ios::in);
	if (!file_.is_open()) throw std::runtime_error("Failed to open the input file");
	file_.read(reinterpret_cast<char*>(&header_),sizeof(wav_header_t));
	if (file_.gcount()!=sizeof(wav_header_t))
		throw std::runtime_error("Failed to read wav header");
	params_.rate = convert_int_to_rate(header_.rate);
	if (header_.bps != 16) {
		throw std::runtime_error("Only 16bit depths supported");
	}
	if (header_.channels != 2 ) {
		if (header_.channels == 1) {
			mono_source_ = true;
		} else {
			throw std::runtime_error("Only mono or stereo samples are supported");
		}
	}
}

void WaveFile::update(size_t new_data_size)
{
	header_.add_size(static_cast<uint32_t>(new_data_size));
	file_.seekp(0,std::ios::beg);
	file_.write(reinterpret_cast<char*>(&header_),sizeof(header_));
	file_.seekp(0,std::ios::end);
}

audio_params_t WaveFile::get_params() const
{
	return params_;
}

error_type_t WaveFile::store_data(const std::vector<audio_sample_t>& data, size_t sample_count)
{
	if (!sample_count) sample_count = data.size();
	const size_t data_size = sample_count*params_.sample_size();
	if (data_size) {
		update(data_size);
		file_.write(reinterpret_cast<const char*>(&data[0]),data_size);
	}
	return error_type_t::ok;
}

error_type_t WaveFile::read_data(std::vector<audio_sample_t>& data, size_t& sample_count)
{
	size_t max_samples = data.size();
	if (sample_count > max_samples) sample_count = max_samples;
	if (!mono_source_) {
		file_.read(reinterpret_cast<char*>(&data[0]),sample_count*params_.sample_size());
		sample_count = file_.gcount() / params_.sample_size();
	} else {
		mono_buffer_.resize(sample_count);
		file_.read(reinterpret_cast<char*>(&mono_buffer_[0]),sample_count*sizeof(int16_t));
		sample_count = file_.gcount() / sizeof(int16_t);
		std::copy(mono_buffer_.begin(), mono_buffer_.begin() + sample_count, data.begin());
	}

	return error_type_t::ok;
}
}

