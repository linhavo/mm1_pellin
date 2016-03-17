/*!
 * @file 		Socket.h
 * @author 		Zdenek Travnicek <travnicek@iim.cz>
 * @date 		13.11.2013
 * @copyright	Institute of Intermedia, 2013 - 2016
 * 				GNU Public License 3.0
 *
 */


#ifndef NET_H_
#define NET_H_
#include <string>
#include <cstdint>
#include <vector>
#include "iimavlib/PlatformDefs.h"

#ifdef SYSTEM_WINDOWS
#include <cstdint>
#ifndef MODERN_COMPILER
#define noexcept throw()
#endif
//typedef int64_t ssize_t;
#endif

#ifdef SYSTEM_WINDOWS
#include <winsock2.h>
#include <WS2tcpip.h>
#endif


namespace iimavlib {
namespace artnet {

class Socket {
public:
#ifdef SYSTEM_WINDOWS
	typedef SOCKET socket_type;
#else
	typedef int	socket_type;
#endif
	Socket(int type, uint16_t listen_port = 0);
	virtual ~Socket() noexcept;
	Socket(Socket&&);
	Socket& operator=(Socket&&);
#ifndef SYSTEM_WINDOWS
	Socket(const Socket&) = delete;
	Socket& operator=(const Socket&) = delete;
#endif
	/*!
	 * Sends data to connected socket
	 * @param data Pointer to data to send
	 * @param size Size of data in bytes
	 * @return true if some data were sent, false otherwise.
	 */
	bool send(const uint8_t* data, size_t size);

	/*!
	 * Receives data from connected socket.
	 * @param data Pointer to memory buffer to store the data
	 * @param size Size of the buffer
	 * @return Number of bytes read
	 * @throw Throws std::runtime_exception if there's any problem with connection
	 */
	size_t receive(uint8_t* data, size_t size);


	bool ready_to_receive();
	void close() noexcept;
	/* **********************************************************************
	 *  Convenience wrappers for send/receive
	 ********************************************************************** */

	template<class T>
	bool send(const T* data, size_t count) {
		return send(reinterpret_cast<const uint8_t*>(data), sizeof(T) * count);
	}

	template<class T>
	bool send(const std::vector<T>& data) {
		return send(reinterpret_cast<const uint8_t*>(data.data()), sizeof(T)*data.size());
	}
	template<class T>
	bool send(const T& data) {
		return send(reinterpret_cast<const uint8_t*>(&data), sizeof(T));
	}


	template<class T>
	size_t receive(T* data, size_t count) {
		return receive(reinterpret_cast<uint8_t*>(data), sizeof(T) * count);
	}

	template<class T>
	size_t receive(std::vector<T>& data) {
		size_t read = receive(reinterpret_cast<uint8_t*>(data.data()), sizeof(T)*data.size());
		data.resize(read/sizeof(T));
		return read/sizeof(T);
	}
	template<class T>
	size_t receive(T& data) {
		size_t read = receive(reinterpret_cast<uint8_t*>(&data), sizeof(T));
		return read/sizeof(T);
	}

//private:
//	virtual bool do_send(const uint8_t* data, size_t size) = 0;
//	virtual size_t do_receive(uint8_t* data, size_t size) = 0;
protected:
	Socket();
	socket_type socket_;
};

}

}


#endif /* NET_H_ */
