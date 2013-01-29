/**
 * @file 	WaveFile.h
 *
 * @date 	19.1.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 */

#ifndef WAVEFILE_H_
#define WAVEFILE_H_

#include "AudioTypes.h"
#include "PlatformDefs.h"
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
namespace iimaudio {
enum class read_mode_t {
	read, write
};

PACKED_PRE
struct wav_header_t
{
	char cID[4];
	uint32_t cSize;
	char wavID[4];
	char subID[4];
	uint32_t subSize;
	uint16_t fmt;
	uint16_t channels;
	uint32_t rate;
	uint32_t byte_rate;
	uint16_t block_align;
	uint16_t bps;
	char dataID[4];
	uint32_t dataSize;
	wav_header_t(uint16_t channels=2,uint32_t rate=44100,uint16_t bps=16,bool le=true):
			cSize(0),
			subSize(16),fmt(1),channels(channels),rate(rate),
			byte_rate((rate*channels*bps)>>3),block_align((channels*bps)>>3),bps(bps),
			dataSize(0)
	{
		// Ugly stupid workaround for visual studio stupidity
		//set_array("RIFF",cID,4);
		std::copy_n("RIFF",4,cID);
		if (!le) cID[3]='X';
		std::copy_n("WAVE",4,wavID);
		std::copy_n("fmt ",4,subID);
		std::copy_n("data",4,dataID);
	}
	void add_size(uint32_t size) { dataSize+=size;cSize=36+dataSize; }

} PACKED;


class WaveFile
{
	WaveFile() = delete;
public:
	/**
	 * @brief Constructor for writing a WAV file
	 *
	 * Throws std::runtime_exception if it faild to create the file
	 * @param filename Name of the file to write to
	 * @param params Parameters of the WAV file
	 */
	WaveFile(const std::string& filename, audio_params_t params);

	/**
	 * @brief Constructor for reading a WAV file
	 *
	 * Throws std::runtime_exception when the file doesn't exist or the header is corrupted
	 * @param filename Name of the file to read
	 */
	WaveFile(const std::string& filename);


	/**
	 * @brief Adds data to the WAV file
	 * @param data Buffer containing samples for writing
	 * @param sample_count Number of samples in the buffer. Set to 0 to use the whole buffer.
	 * @return Returns return_type_t::ok when written successfully
	 */
	template<typename T>
	return_type_t store_data(const std::vector<T>& data, size_t sample_count = 0);

	/**
	 * @brief
	 * @param data
	 * @param sample_count
	 * @return
	 */
	template<typename T>
	return_type_t read_data(std::vector<T>& data, size_t& sample_count);
	/**
	 * @brief Returns params corresponding to current file
	 * @return Struct containing relevant parameters of the data in WAV file
	 */
	audio_params_t get_params() const;

private:
	wav_header_t header_;
	audio_params_t	params_;
	std::fstream file_;

	void update(size_t new_data_size = 0);

};


template<typename T>
return_type_t WaveFile::store_data(const std::vector<T>& data, size_t sample_count)
{
	if (!sample_count) sample_count = data.size();
	const size_t data_size = sample_count*sizeof(T) * params_.num_channels;
	update(data_size);
	file_.write(reinterpret_cast<const char*>(&data[0]),data_size);
	return return_type_t::ok;
}

template<typename T>
	return_type_t WaveFile::read_data(std::vector<T>& data, size_t& sample_count)
{
	size_t max_samples = data.size() * sizeof(T) / params_.sample_size();
	if (sample_count > max_samples) sample_count = max_samples;
	file_.read(reinterpret_cast<char*>(&data[0]),sample_count*params_.sample_size());
	sample_count = file_.gcount() / params_.sample_size();
	return return_type_t::ok;
}
}

#endif /* WAVEFILE_H_ */
