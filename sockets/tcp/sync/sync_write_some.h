#pragma once

#include <boost/asio.hpp>
#include <cstdint>
#include <iostream>
#include <string>

/// This snippet is meant to be compared with sync_write.h in this same folder
/// You clearly see that if you use write_some you are responsible of handling
/// the situation where the buffer was not sent completely but only partially.
namespace TcpSyncWriteSome
{
	void write_some_to_socket(boost::asio::ip::tcp::socket& socket)
	{
		std::string message_buffer = "This is a simple string to be sent from C++ Boost Asio to a server";
		std::size_t bytes_written = 0;

		// write_some will write as much as it can synchronously
		// we must make sure by ourselves the whole buffer is written, so keep looping
		// in this demo, it is most likely it will write everything in one go, because
		// the message is small, but in large messages(for example when you send a file)
		// you can't sent the whole file in one single write_some(), that is where you really
		// need use a loop.
		while (bytes_written < message_buffer.length())
		{
			bytes_written += socket.write_some(boost::asio::buffer(
				message_buffer.c_str() + bytes_written, message_buffer.length() - bytes_written));
			std::cout << "Sent " << (bytes_written / message_buffer.size() * 100) << "% of bytes so far" << std::endl;
		}
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
			// Setup the endpoint to connect to
			boost::asio::ip::tcp::endpoint ep(
				boost::asio::ip::address::from_string(host_address), port_number);

			boost::asio::io_service io_service;
			boost::asio::ip::tcp::socket socket(io_service, ep.protocol());

			// connect
			socket.connect(ep);

			write_some_to_socket(socket);
		}
		catch (boost::system::system_error& error)
		{
			std::cout << "Error (" << error.code() << ") : " << error.what();
			return error.code().value();
		}

		return 0;
	}
}
