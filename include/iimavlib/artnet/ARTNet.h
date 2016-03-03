/*
 * ARTNet.h
 *
 *  Created on: 30.1.2014
 *      Author: neneko
 */

#ifndef ARTNET_H_
#define ARTNET_H_
#include <cstdint>
#include <vector>
#include "Socket.h"

namespace iimavlib {
namespace artnet {

class Packet {
public:
	Packet();
	~Packet() noexcept;
	uint8_t& operator[] (uint16_t index);
	uint8_t operator[] (uint16_t index) const;

	bool send(Socket& socket);
private:
	std::vector<uint8_t> data_;


};

}
}
#endif /* ARTNET_H_ */
