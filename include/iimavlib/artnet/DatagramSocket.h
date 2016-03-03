/*
 * DatagramSocket.h
 *
 *  Created on: 30.1.2014
 *      Author: neneko
 */

#ifndef STREAMSOCKET_H_
#define STREAMSOCKET_H_

#include "Socket.h"


namespace iimavlib {
namespace artnet {


class DatagramSocket: public Socket
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
#ifndef THEREMIN_WINDOWS
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
