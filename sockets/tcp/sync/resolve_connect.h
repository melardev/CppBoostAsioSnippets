#pragma once
#include <boost/asio.hpp>
#include <iostream>

namespace TcpSyncClientResolveConnect
{
	int main()
	{
		// prepare a server listener running ncat(install Nmap before), or nc if you are on Linux
		// ncat -lv 3002
		// nc -lv 3002
		// Define Server hostname and Server port

		std::string hostname = "localhost"; // replace it with a hostname if you have
		std::string port = "3002";

		// io_service the core of Boost asio
		boost::asio::io_service ios;

		// Create a DNS query
		boost::asio::ip::tcp::resolver::query resolver_query(hostname, port,
		                                                     boost::asio::ip::tcp::resolver::query::numeric_service);

		// Create a resolver.
		boost::asio::ip::tcp::resolver resolver(ios);

		try
		{
			// Make a DNS Query, retrieve the DNS Query response as an iterator of hosts
			boost::asio::ip::tcp::resolver::iterator it = resolver.resolve(resolver_query);

			// Create client socket which is the connection object
			boost::asio::ip::tcp::socket clientSocket(ios);

			// Connect to the host, provide the list of hosts retrieved from the DNS query
			// it will try to connect to first one, if it fails, it tries the second, etc
			// if all fail, then it raises an exception
			boost::asio::connect(clientSocket, it);

			boost::asio::write(clientSocket, boost::asio::buffer("Hello world"));
		}
		catch (boost::system::system_error& e)
		{
			std::cout << "Error occured! Error code = " << e.code()
				<< ". Message: " << e.what();

			return e.code().value();
		}

		return 0;
	}
}
