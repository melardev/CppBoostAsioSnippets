#pragma once
#include <boost/asio.hpp>
#include <iostream>

namespace BoostAsioGetHostName
{
	int main()
	{
		std::cout << boost::asio::ip::host_name() << std::endl;
		return 0;
	}
}
