/**
 * @file 	ARTNet.cpp
 *
 * @date 	30.1.2014
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 */


#include "iimavlib/artnet/ARTNet.h"
#include <array>
#include <stdexcept>

namespace iimavlib {
namespace artnet {

namespace {
#ifdef MODERN_COMPILER
	constexpr uint16_t max_values = 512;
	constexpr uint16_t header_size = 18;
	constexpr uint16_t sequence_offset = 12;
#else
	const uint16_t max_values = 512;
	const uint16_t header_size = 18;
	const uint16_t sequence_offset = 12;
#endif
#ifdef MODERN_COMPILER
	const std::array<uint8_t,header_size> default_artnet_header
	{{'A','r','t','-','N','e','t',0, // Magic
	 0x00, 0x50, 	// Opcode
	 0x00, 14,		// Version
	 0x00,			// Sequence
	 0x00,			// Physical if
	 0x00, 0x00		// Length
	}};
#else
	// Compiling under VS2012 or older... that mean no support for most of c++11 features, so we need to fall back to old C arrays ;/
	const uint8_t default_artnet_header[header_size] =
	{'A','r','t','-','N','e','t',0, // Magic
	 0x00, 0x50, 	// Opcode
	 0x00, 14,		// Version
	 0x00,			// Sequence
	 0x00,			// Physical if
	 0x00, 0x00		// Length
	};

#endif
}
Packet::Packet():data_(header_size,0)
{
	//std::copy(default_artnet_header.begin(),default_artnet_header.end(),data_.begin());
	std::copy(std::begin(default_artnet_header),std::end(default_artnet_header),data_.begin());
}
Packet::~Packet() noexcept
{

}

uint8_t& Packet::operator[] (uint16_t index)
{
	const uint16_t array_index = index + header_size;
	if (array_index >= data_.size()) {
		if (index > max_values) {
			throw std::out_of_range("Index out of range");
		}
		data_.resize(array_index+1,0);
		data_[header_size-1]=index&0xFF;
		data_[header_size-2]=(index>>8)&0xFF;
	}
	return data_[array_index];
}
uint8_t Packet::operator[] (uint16_t index) const
{
	const uint16_t array_index = index + header_size;
	if (array_index >= data_.size()) throw std::out_of_range("Index out of range");
	return data_[array_index];
}

bool Packet::send(Socket& socket)
{
	if (socket.send(data_)) {
		data_[sequence_offset] = (data_[sequence_offset]+1)&0xFF;
		return true;
	}
	return false;


}

size_t Packet::size() const
{
	return data_.size() - header_size;
}
}
}
