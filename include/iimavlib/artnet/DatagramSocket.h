/*!
 * @file 		DatagramSocket.h
 * @author 		Zdenek Travnicek <travnicek@iim.cz>
 * @date 		30.1.2014
 * @copyright	Institute of Intermedia, 2014 - 2016
 * 				GNU Public License 3.0
 *
 */


#ifndef STREAMSOCKET_H_
#define STREAMSOCKET_H_

#include "Socket.h"


namespace iimavlib {
namespace artnet {


class EXPORT DatagramSocket: public Socket
{
public:
	/*!
	 * Constructor connecting to a specified ip and port
	 * @param ip
	 * @param port
	 */
	DatagramSocket(const std::string& ip, const uint16_t port);

	/*!
	 * Constructor creating listening socket
	 * @param ip
	 * @param port
	 */
	DatagramSocket(const uint16_t port);
	DatagramSocket(DatagramSocket&&);

	DatagramSocket& operator=(DatagramSocket&&);
#ifdef MODERN_COMPILER
	DatagramSocket(const DatagramSocket&) = delete;
	DatagramSocket& operator=(const DatagramSocket&) = delete;
#endif
	virtual ~DatagramSocket() noexcept;

	DatagramSocket accept_connection();
private:
	// default constructor, only for private use
	DatagramSocket();
};


}
}


#endif /* STREAMSOCKET_H_ */
