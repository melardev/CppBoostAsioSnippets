#pragma once

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/make_shared.hpp>

namespace TcpAsyncServerEchoOOPSharedSimple
{
	inline std::string make_daytime_string()
	{
		using namespace std; // For time_t, time and ctime;
		time_t now = time(0);
		return ctime(&now);
	}

	class TcpAsyncClientConnection
		: public boost::enable_shared_from_this<TcpAsyncClientConnection>
	{
	public:

		static boost::shared_ptr<TcpAsyncClientConnection> create(boost::asio::io_service& io_context)
		{
			return boost::shared_ptr<TcpAsyncClientConnection>(new TcpAsyncClientConnection(io_context));
		}

		boost::asio::ip::tcp::socket& socket()
		{
			return socket_;
		}

		void start_async()
		{
			running_ = true;
			read_async();
		}

		void read_async()
		{
			boost::asio::async_read(socket_, boost::asio::buffer(receive_buffer_, 1024),
			                        boost::bind(&TcpAsyncClientConnection::handle_read, shared_from_this(),
			                                    boost::asio::placeholders::error,
			                                    boost::asio::placeholders::bytes_transferred));
		}

		void handle_read(const boost::system::error_code& ec, size_t bytes_transferred)
		{
			if (ec)
			{
				return stop();
			}
			else
			{
				const std::string message(receive_buffer_);
				write_async_message(message);
			}
		}

		void write_async_message(const std::string& message)
		{
			boost::asio::async_write(socket_, boost::asio::buffer(message),
			                         boost::bind(&TcpAsyncClientConnection::handle_write, shared_from_this(),
			                                     boost::asio::placeholders::error,
			                                     boost::asio::placeholders::bytes_transferred));
		}

	private:
		TcpAsyncClientConnection(boost::asio::io_service& io_context)
			: socket_(io_context)
		{
		}

		void handle_write(const boost::system::error_code& ec,
		                  size_t /*bytes_transferred*/)
		{
			if (ec)
			{
				return stop();
			}
			else
			{
				read_async();
			}
		}

		void stop()
		{
			if (!running_) return;
			running_ = false;
			socket_.close();
		}

		bool running_;
		boost::asio::ip::tcp::socket socket_;
		char receive_buffer_[1024];
	};

	class TcpAsyncServerOOPSharedBasic : public boost::enable_shared_from_this<TcpAsyncServerOOPSharedBasic>
	{
	public:


		TcpAsyncServerOOPSharedBasic(boost::asio::io_context& io_context)
			: acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 3002))
		{
		}

		static boost::shared_ptr<TcpAsyncServerOOPSharedBasic> create(boost::asio::io_service& ios)
		{
			// make_shared has many advantages, but ideally, you should never be able to use it, why?
			// because a class that uses shared_from_this(), which I do in this class, should guarantee
			// that the only way to create an instance of this class, should be through a public static method:
			// and make the constructor private, if the constructor is private make_shared can not perform its work

			// If you try to run shared_from_this(), but there is no alive boost::shared_ptr<this class> in the app
			// then the app will crash, this is basics on shared_ptr, review that if you have hard to understand it.
			return boost::make_shared<TcpAsyncServerOOPSharedBasic>(ios);
		}

		void start_async()
		{
			accept_one_client_async();
		}

	private:


		void accept_one_client_async()
		{
			// Create a shared pointer to client
			auto new_connection = TcpAsyncClientConnection::create(acceptor_.get_executor().context());

			acceptor_.async_accept(new_connection->socket(),
			                       boost::bind(&TcpAsyncServerOOPSharedBasic::handle_accept, shared_from_this(),
			                                   new_connection,
			                                   boost::asio::placeholders::error));
		}

		void handle_accept(boost::shared_ptr<TcpAsyncClientConnection> client,
		                   const boost::system::error_code& error)
		{
			if (!error)
			{
				client->start_async();
			}

			accept_one_client_async();
		}

		boost::asio::ip::tcp::acceptor acceptor_;
	};

	int main()
	{
		boost::asio::io_service ios;
		auto server = TcpAsyncServerOOPSharedBasic::create(ios);
		server->start_async();
		ios.run(); // keep looping and handling network events
		return 0;
	}
}
