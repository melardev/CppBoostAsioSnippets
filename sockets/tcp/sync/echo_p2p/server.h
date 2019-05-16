#pragma once
#include <boost/asio.hpp>
#include <iostream>
#include <string>

namespace TcpSyncServerEchoOOPP2P
{
	class TcpServerEchoP2P
	{
	public:
		TcpServerEchoP2P()
			: server_socket_(io_service_), acceptor_(io_service_), receiving_buffer_()
		{
		}

		void prepare(const char* host_address = "localhost", uint16_t port = 3002)
		{
			// configure a local endpoint using a resolver
			boost::asio::ip::tcp::resolver resolver(io_service_);
			const boost::asio::ip::tcp::resolver::query dns_query(host_address, "3002");
			boost::asio::ip::tcp::resolver::iterator dns_response_iterator = resolver.resolve(dns_query);
			const boost::asio::ip::tcp::resolver::iterator end_iterator;

			// Iterate through the query responses
			while (dns_response_iterator != end_iterator)
			{
				boost::asio::ip::tcp::endpoint endpoint = *dns_response_iterator++;
				// Get the first responses that has an IP v4
				if (endpoint.address().is_v4())
				{
					server_address_ = endpoint;
					break;
				}
			}
		}

		void run()
		{
			acceptor_.open(server_address_.protocol());
			acceptor_.bind(server_address_);
			acceptor_.listen();

			std::cout << "Listening on "
				<< acceptor_.local_endpoint().address().to_string().c_str()
				<< ":" << acceptor_.local_endpoint().port() << std::endl << std::endl;
			running_ = true;
			// accept synchronously
			acceptor_.accept(server_socket_);
			receiving_buffer_.fill(0x00);
			while (running_)
			{
				try
				{
					// read synchronously
					size_t bytes_read = server_socket_.read_some(boost::asio::buffer(receiving_buffer_));

					std::cout << "Message received ("
						<< server_socket_.remote_endpoint().address().to_string().c_str() << ":"
						<< server_socket_.remote_endpoint().port() << "): "
						<< receiving_buffer_.data() << std::endl;

					// Echo the message back
					std::string message = receiving_buffer_.data();
					size_t bytes_written = server_socket_.write_some(boost::asio::buffer(message));

					receiving_buffer_.fill(0x00);
				}
				catch (boost::system::system_error& err)
				{
					std::cout << "An error occured" << std::endl;
					break;
				}
			}
			// Close the server socket
			server_socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
			server_socket_.close();
		}

	private:
		boost::asio::io_service io_service_;
		boost::asio::ip::tcp::socket server_socket_;
		boost::asio::ip::tcp::acceptor acceptor_;

		boost::asio::ip::tcp::endpoint server_address_;
		boost::asio::ip::tcp::endpoint remote_endpoint_;

		static constexpr size_t BUFFER_SIZE = 1024;
		std::array<char, BUFFER_SIZE> receiving_buffer_;

		bool running_;
	};


	void main()
	{
		TcpServerEchoP2P server;
		server.prepare();
		server.run();
	}
}
