#pragma once
#include <string>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/core/noncopyable.hpp>
#include <boost/make_shared.hpp>

/// Crazy demo, I don't remember who inspired me for this snippet
/// but it is something I would never user, it is about an async Echo server
/// with multiple clients support, the weird thing is on the boost::asio::async_read
/// overload method used, I used one that triggers a function after EACH SINGLE BYTE read
/// to check if the packet is complete, returning true if so. If we return true, boost will call
/// the second handler specified as arg with std::bind().
/// I have hard to imagine how efficient this would be in a big app, I prefer to handle if the
/// message is complete or not without having to get notified byte by byte what asio has just read...
/// anyways, learn from this.
namespace TcpAsyncServerEchoOOPShared
{
	// Server App: Server and Server connection(one per user)
	class AsyncClientConnection : public boost::enable_shared_from_this<AsyncClientConnection>,
	                              public boost::noncopyable
	{
	public:
		AsyncClientConnection(boost::asio::io_service& io_service) : sock_(io_service), running_(false)
		{
		}

		static boost::shared_ptr<AsyncClientConnection> create(boost::asio::io_context& context)
		{
			return boost::make_shared<AsyncClientConnection>(context);
		}


		void start_async()
		{
			running_ = true;
			read_message_async();
		}


		void stop()
		{
			if (!running_) return;
			running_ = false;
			sock_.close();
		}

		boost::asio::ip::tcp::socket& sock() { return sock_; }
	private:


		void read_message_async()
		{
			boost::asio::async_read(sock_, boost::asio::buffer(read_buffer_),
			                        boost::bind(&AsyncClientConnection::on_read_some, shared_from_this(),
			                                    boost::asio::placeholders::error,
			                                    boost::asio::placeholders::bytes_transferred),
			                        boost::bind(&AsyncClientConnection::on_read_complete, shared_from_this(),
			                                    boost::asio::placeholders::error,
			                                    boost::asio::placeholders::bytes_transferred));
		}

		size_t on_read_some(const boost::system::error_code& err, size_t bytes)
		{
			if (err) return 0;
			bool found = std::find(read_buffer_, read_buffer_ + bytes, '\n') < read_buffer_ + bytes;
			return found ? 0 : 1;
		}

		void on_read_complete(const boost::system::error_code& ec, size_t bytes)
		{
			if (!ec)
			{
				const std::string msg(read_buffer_, bytes);
				std::cout << "Received " << msg << std::endl;
				std::cout << "Echoing it back" << std::endl;
				// echo message back, and then stop
				do_write(msg + "\n");
				return;
			}
			stop();
		}


		void do_write(const std::string& message)
		{
			sock_.async_write_some(boost::asio::buffer(message.c_str(), message.size()),
			                       boost::bind(&AsyncClientConnection::on_write, shared_from_this(),
			                                   boost::asio::placeholders::error,
			                                   boost::asio::placeholders::bytes_transferred));
		}

		void on_write(const boost::system::error_code& err, size_t bytes)
		{
			read_message_async();
		}

	private:
		boost::asio::ip::tcp::socket sock_;

		char read_buffer_[1024];

		bool running_;
	};


	class TcpAsyncServerOOPShared : public boost::enable_shared_from_this<TcpAsyncServerOOPShared>
	{
	public:
		TcpAsyncServerOOPShared(boost::asio::io_service& ios) : acceptor(ios,
		                                                                 boost::asio::ip::tcp::endpoint(
			                                                                 boost::asio::ip::tcp::v4(), 3002))
		{
		}

		static boost::shared_ptr<TcpAsyncServerOOPShared> create(boost::asio::io_service& ios)
		{
			return boost::make_shared<TcpAsyncServerOOPShared>(ios);
		}

		void start()
		{
			accept_one_client();
		}

	private:
		void handle_accept(boost::shared_ptr<AsyncClientConnection> client, const boost::system::error_code& err)
		{
			if (!err)
			{
				client->start_async();
			}
			// Accept another client
			accept_one_client();
		}


		void accept_one_client()
		{
			boost::shared_ptr<AsyncClientConnection> future_new_connection
				= AsyncClientConnection::create(acceptor.get_executor().context());

			//boost::shared_ptr<TcpAsyncClient2> future_new_connection(new TcpAsyncClient2(acceptor.get_executor().context()));

			acceptor.async_accept(future_new_connection->sock(),
			                      boost::bind(&TcpAsyncServerOOPShared::handle_accept, shared_from_this(),
			                                  future_new_connection,
			                                  boost::asio::placeholders::error));
		}

	private:
		boost::asio::ip::tcp::acceptor acceptor;
	};

	int main()
	{
		boost::asio::io_service ios_service;
		auto server = TcpAsyncServerOOPShared::create(ios_service);
		server->start();
		ios_service.run(); // Keep looping and handling tasks
		return 0;
	}
}
