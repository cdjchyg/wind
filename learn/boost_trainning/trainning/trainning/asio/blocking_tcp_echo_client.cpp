
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

enum { max_length = 1024 };

int test_main()
{
	try
	{

		boost::asio::io_service io_service;

		tcp::resolver resolver(io_service);
		tcp::resolver::query query(tcp::v4(), "127.0.0.1", "2222");
		tcp::resolver::iterator iterator = resolver.resolve(query);

		tcp::socket s(io_service);
		boost::asio::connect(s, iterator);

		using namespace std; // For strlen.
		std::cout << "Enter message: ";
		char request[max_length];
		std::cin.getline(request, max_length);
		size_t request_length = strlen(request);
		boost::asio::write(s, boost::asio::buffer(request, request_length));

		char reply[max_length];
		size_t reply_length = boost::asio::read(s,
			boost::asio::buffer(reply, request_length));
		std::cout << "Reply is: ";
		std::cout.write(reply, reply_length);
		std::cout << "\n";
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}