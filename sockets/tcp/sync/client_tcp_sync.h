#pragma once
#include <boost/asio.hpp>
#include <iostream>


namespace TcpSyncConnect
{
	int main()
	{
		// ncat -lv -k 3002
		// -v verbose -l listen mode
		// Server Ip and port, no need to resolve
		std::string ip_address_str = "127.0.0.1";
		unsigned short port = 3002;

		try
		{
			// Create the endpoint from the ip string
			boost::asio::ip::tcp::endpoint
				ep(boost::asio::ip::address::from_string(ip_address_str),
				   port);

			boost::asio::io_service ios;

			// Create the socket.
			boost::asio::ip::tcp::socket sock(ios, ep.protocol());

			// Connect
			sock.connect(ep);

			// Overloads of asio::ip::address::from_string() and 
			// asio::ip::tcp::socket::connect() used here throw
			// exceptions in case of error condition.
		}
		catch (boost::system::system_error& e)
		{
			std::cout << "Error occured! Error code = " << e.code()
				<< ". Message: " << e.what();

			return e.code().value();
		}
		return 0;
	}
};
