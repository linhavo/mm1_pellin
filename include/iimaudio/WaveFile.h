/*
 * WaveFile.h
 *
 *  Created on: 19.1.2013
 *      Author: neneko
 */

#ifndef WAVEFILE_H_
#define WAVEFILE_H_

#include "AudioTypes.h"
#include <fstream>
#include <string>
#include <vector>
namespace iimaudio {
enum class read_mode_t {
	read, write
};

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
			cID{'R','I','F','F'}, cSize(0),wavID{'W','A','V','E'},subID{'f','m','t',' '},
			subSize(16),fmt(1),channels(channels),rate(rate),
			byte_rate((rate*channels*bps)>>3),block_align((channels*bps)>>3),bps(bps),
			dataID{'d','a','t','a'},dataSize(0)
	{
				if (!le) cID[3]='X';
	}
	void add_size(uint32_t size) { dataSize+=size;cSize=36+dataSize; }

} __attribute__((packed));


class WaveFile
{
public:
	WaveFile(const std::string& filename, audio_params_t params);
	template<typename T>
	return_type_t store_data(const std::vector<T>& data, size_t sample_count = 0);
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

}

#endif /* WAVEFILE_H_ */
