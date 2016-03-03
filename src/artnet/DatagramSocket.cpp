/*
 * DatagramSocket.cpp
 *
 *  Created on: 30.1.2014
 *      Author: neneko
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

#include <iostream>
#include <stdexcept>

namespace iimavlib {
namespace artnet {

DatagramSocket::DatagramSocket():Socket()
{}

DatagramSocket::DatagramSocket(const std::string& ip, const uint16_t port):
Socket(SOCK_DGRAM)
{
	struct sockaddr_in in;

	std::cerr << "Connecting to server\n";
	in.sin_family = AF_INET;
	in.sin_port = htons(port);
	in.sin_addr.s_addr = inet_addr(ip.c_str());
	if (connect (socket_,reinterpret_cast<const sockaddr*>(&in),sizeof(in))==-1) {
		std::cerr << "Failed with code " << errno << "\n";
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


