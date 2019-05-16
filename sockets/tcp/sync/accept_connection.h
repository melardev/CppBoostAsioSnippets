#pragma once
#include <boost/asio.hpp>
#include <iostream>


namespace TcpSyncAcceptConnection
{
	int main()
	{
		unsigned short port = 3002;
		// Get the ip address belonging to any Network Interface card in this machine
		boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address_v4::any(), port);

		boost::asio::io_service ios;

		try
		{
			int BACKLOG_SIZE = 5;
			// Create the socket acceptor
			boost::asio::ip::tcp::acceptor serverAcceptor(ios, endpoint.protocol());

			// bind the Acceptor to the Network Interface Address we retrieved before
			serverAcceptor.bind(endpoint);

			// Start listening
			serverAcceptor.listen(BACKLOG_SIZE);

			// The acceptor needs a Server Socket to work on, create it
			boost::asio::ip::tcp::socket clientSocket(ios);
			// make the acceptor use the server socket and accept connections

			// To test this run in your shell
			// ncat localhost 3003
			// You would need ncat which ships with nmap, or if you are on linux, use:
			// nc localhost 3002
			serverAcceptor.accept(clientSocket); // it is sync, this may throw a system_error exception

			clientSocket.shutdown(boost::asio::socket_base::shutdown_both);
			boost::system::error_code ec;
			clientSocket.close(ec);

			if (ec) // operator bool() is called
			{
				std::cout << ec.message() << std::endl;
			}
		}
		catch (boost::system::system_error& e)
		{
			std::cout << "Error code: " << e.code() << " Error Message" << e.what() << std::endl;
		}
		return 0;
	}
}
