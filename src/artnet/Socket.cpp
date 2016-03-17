/*!
 * @file 		Socket.cpp
 * @author 		Zdenek Travnicek <travnicek@iim.cz>
 * @date 		13.11.2013
 * @copyright	Institute of Intermedia, 2013 - 2016
 * 				GNU Public License 3.0
 *
 */

#include "iimavlib/artnet/Socket.h"
#include "iimavlib/Utils.h"
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
#include <cstring>


namespace iimavlib {
namespace artnet {
namespace {

#ifdef SYSTEM_WINDOWS
bool wsa_initialized = false;
void init_wsa()
{
	if (wsa_initialized) return;
	WORD req;
	WSADATA data;
	req = MAKEWORD(2,2);
	if (WSAStartup(req, &data) != 0 ) {
		throw std::runtime_error("Failed to init wsa");
	}
	if (LOBYTE(data.wVersion) != 2 || HIBYTE(data.wVersion) != 2) {
		throw std::runtime_error("Failed to init wsa");
	}

	wsa_initialized = true;
}
#endif



Socket::socket_type prepare_socket(uint16_t port, int type = SOCK_STREAM)
{
#ifdef THEREMIN_WINDOWS
	init_wsa();
#endif
	struct sockaddr_in out;
	logger[log_level::debug] << "Creating socket";
	Socket::socket_type sock = ::socket(AF_INET, type, 0);
	if (!sock) throw std::runtime_error("Failed to allocate socket");
	out.sin_family = AF_INET;
	out.sin_port = htons(port);
	out.sin_addr.s_addr = INADDR_ANY;
	std::cerr << "Binding socket " << "\n";
	if (::bind(sock,reinterpret_cast<const sockaddr*>(&out),sizeof(out))==-1) {
		logger[log_level::fatal] << "Failed with code " << errno << "(" << std::strerror(errno) << ")";
		throw std::runtime_error("Failed to bind socket");
	}
	return sock;
}
}

Socket::Socket(int type, uint16_t listen_port):socket_(-1)
{
	logger[log_level::debug] << "Creating socket";
	socket_ = prepare_socket(listen_port, type);
}

Socket::Socket():socket_(-1)
{

}
Socket::~Socket() noexcept {
	close();
}

Socket::Socket(Socket&& other)
:socket_(other.socket_)
{
	other.socket_ = -1;
}

Socket& Socket::operator=(Socket&& other)
{
	close();
	std::swap(socket_, other.socket_);
	return *this;
}


void Socket::close() noexcept
{
	if (socket_>0) {
		logger[log_level::debug] << "Deleting socket";
#ifndef SYSTEM_WINDOWS
		::close(socket_);
#else
		closesocket(socket_);
#endif
		socket_ = -1;
	}
}


bool Socket::send(const uint8_t* data, size_t size)
{
	// Explicit cast needed for windows sockets
	return ::send(socket_,reinterpret_cast<const char*>(data),static_cast<int>(size), 0) >= 0;
}

size_t Socket::receive(uint8_t* data, size_t size)
{
	// Explicit cast needed for windows sockets
	const auto read = ::recv(socket_, reinterpret_cast<char*>(data), static_cast<int>(size), 0);
	if (read < 0) throw std::runtime_error("Failed to read from socket");
	return read;
}

bool Socket::ready_to_receive()
{
	pollfd pfd = {socket_, POLLIN, 0};
#ifndef SYSTEM_WINDOWS
	return ::poll(&pfd, 1, 0) > 0;
#else
	return WSAPoll(&pfd, 1, 0) > 0;
#endif
}
}
}



