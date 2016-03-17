/*!
 * @file 		DatagramSocket.cpp
 * @author 		Zdenek Travnicek <travnicek@iim.cz>
 * @date 		30.1.2014
 * @copyright	Institute of Intermedia, 2014 - 2016
 * 				GNU Public License 3.0
 *
 */

#include "iimavlib/artnet/DatagramSocket.h"
#ifdef SYSTEM_WINDOWS
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>
#endif
#include "iimavlib/Utils.h"
#include <iostream>
#include <stdexcept>
#include <cstring>

namespace iimavlib {
namespace artnet {

DatagramSocket::DatagramSocket():Socket()
{}

DatagramSocket::DatagramSocket(const std::string& ip, const uint16_t port):
Socket(SOCK_DGRAM)
{
	struct sockaddr_in in;

	logger[log_level::debug] << "Connecting to server";
	in.sin_family = AF_INET;
	in.sin_port = htons(port);
	auto ret = ::inet_pton(AF_INET, ip.c_str(), &in.sin_addr);
	if (ret < 0) {
		logger[log_level::fatal] << "Failed to parse IP address " << ip;
	}
	if (::connect (socket_,reinterpret_cast<const sockaddr*>(&in),sizeof(in))==-1) {
		logger[log_level::fatal] << "Failed with code " << errno << "(" << std::strerror(errno) << ")";
		throw std::runtime_error("Failed to connect to server");
	}
}

DatagramSocket::DatagramSocket(const uint16_t port):Socket(SOCK_DGRAM, port)
{
	::listen(socket_, 10);
}

DatagramSocket::DatagramSocket(DatagramSocket&& other)
:Socket(std::move(other))
{
}

DatagramSocket::~DatagramSocket() noexcept
{

}
DatagramSocket& DatagramSocket::operator=(DatagramSocket&& other)
{
	Socket::operator=(std::move(other));
	return *this;
}

DatagramSocket DatagramSocket::accept_connection()
{
	DatagramSocket s;
	struct sockaddr_in in;
	socklen_t len = sizeof(in);
	s.socket_ = ::accept(socket_, reinterpret_cast<sockaddr*>(&in), &len);
	return s;
}

}
}


