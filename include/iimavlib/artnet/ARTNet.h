/**
 * @file 	ARTNet.h
 *
 * @date 	30.1.2014
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 */


#ifndef ARTNET_H_
#define ARTNET_H_
#include <cstdint>
#include <vector>
#include "Socket.h"

namespace iimavlib {
namespace artnet {

class EXPORT Packet {
public:
	Packet();
	~Packet() noexcept;
	uint8_t& operator[] (uint16_t index);
	uint8_t operator[] (uint16_t index) const;
	size_t size() const;
	bool send(Socket& socket);
private:
	std::vector<uint8_t> data_;


};

}
}
#endif /* ARTNET_H_ */
