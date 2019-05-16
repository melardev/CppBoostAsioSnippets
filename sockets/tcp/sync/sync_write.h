#pragma once

#include <boost/asio.hpp>
#include <cstdint>
#include <iostream>
#include <string>

/// This snippet is meant to be compared vs sync_write_some.h
/// When we used write_some we had to take care the whole buffer is delivered
/// whereas in this snippet we let boost asio to do it on our behalf
namespace TcpSyncWriteWholeBuffer
{
	void write_whole_buffer_sync(boost::asio::ip::tcp::socket& socket)
	{
		const std::string message_buffer = "This is a simple string to be sent from C++ Boost Asio to a server";

		// Write whole buffer to the socket.
		// Boost Asio takes care of sending it completely( in write_some snippet)
		// it was different, remember?
		boost::asio::write(socket, boost::asio::buffer(message_buffer));
	}

	int main()
	{
		// Before anything prepare a server, you can run
		// with Nmap installed: ncat -l -p 3002
		// or on Linux: nc -l -p 3002
		std::string host_address = "127.0.0.1";
		uint16_t port_number = 3002;

		try
		{
			boost::asio::ip::tcp::endpoint ep(
				boost::asio::ip::address::from_string(host_address), port_number);

			boost::asio::io_service io_service;
			boost::asio::ip::tcp::socket socket(io_service, ep.protocol());

			socket.connect(ep);

			write_whole_buffer_sync(socket);
		}
		catch (boost::system::system_error& error)
		{
			std::cout << "Error (" << error.code() << ") : " << error.what();
			return error.code().value();
		}

		return 0;
	}
}
