#pragma once
#include <iostream>
#include <boost/asio.hpp>


namespace TcpSyncAcceptReadUntilWrite
{
	std::string read_sync(boost::asio::ip::tcp::socket& sock)
	{
		boost::asio::streambuf buf;
		boost::asio::read_until(sock, buf, "\n");

		std::string data = boost::asio::buffer_cast<const char*>(buf.data());
		return data;
	}


	void write_sync(boost::asio::ip::tcp::socket& sock, const std::string& message)
	{
		const std::string msg = message + "\n";
		boost::asio::write(sock, boost::asio::buffer(message));
	}

	int main()
	{
		// to test this snippet execute it then open in a shell:
		// ncat -v localhost 3002
		// write a text, and then press enter, you will receive the message back
		boost::asio::io_service io_service;

		boost::asio::ip::tcp::acceptor server_acceptor(
			io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 3002));

		boost::asio::ip::tcp::socket socket_(io_service);
		server_acceptor.accept(socket_);

		std::string message = read_sync(socket_);
		std::cout << message << std::endl;

		// Echo it back
		write_sync(socket_, message);

		std::cout << "Message sent to client" << std::endl;

		return 0;
	}
}
