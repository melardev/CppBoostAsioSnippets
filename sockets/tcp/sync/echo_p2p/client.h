#pragma once
#include <boost/asio.hpp>
#include <iostream>


namespace TcpSyncClientEchoOOPP2P
{
	class TcpEchoClient
	{
	public:
		TcpEchoClient()
			: client_socket_(io_service), receiving_buffer_()
		{
		}

		void prepare()
		{
			// configure a remote endpoint (server)
			boost::asio::ip::address ip_address = boost::asio::ip::address::from_string("127.0.0.1");
			remote_endpoint = boost::asio::ip::tcp::endpoint(ip_address, 3002);
		}

		void run()
		{
			client_socket_.open(remote_endpoint.protocol());
			client_socket_.connect(remote_endpoint);

			// send a message
			std::array<char, BUFFER_SIZE> message{0};
			running_ = true;
			while (running_)
			{
				try
				{
					receiving_buffer_.fill(0x00);
					message.fill(0x00);
					std::cout << "Enter the message to send to the server" << std::endl;
					std::cin.getline(message.data(), BUFFER_SIZE);
					size_t bytes_written = client_socket_.write_some(boost::asio::buffer(message));

					// read synchronously
					size_t bytes_read = client_socket_.read_some(boost::asio::buffer(receiving_buffer_));

					std::cout << "received from server("
						<< client_socket_.remote_endpoint().address().to_string().c_str() << ":"
						<< client_socket_.remote_endpoint().port() << "): "
						<< receiving_buffer_.data() << "\n";
				}
				catch (boost::system::system_error& err)
				{
					std::cout << "An error occured" << std::endl;
					break;
				}
			}

			client_socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
			client_socket_.close();
		}


	private
	:
		boost::asio::io_service io_service;
		boost::asio::ip::tcp::socket client_socket_;

		boost::asio::ip::tcp::endpoint local_endpoint;
		boost::asio::ip::tcp::endpoint remote_endpoint;

		bool running_;
		static constexpr size_t BUFFER_SIZE = 1024;
		std::array<char, BUFFER_SIZE> receiving_buffer_;
	};


	int main()
	{
		TcpEchoClient client;
		client.prepare();
		client.run();
		return 0;
	}
}
