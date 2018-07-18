#include <string>
#include <iostream>
#include <boost/foreach.hpp>

int test_main()
{
	std::string s("test string!");

	BOOST_FOREACH(auto x, s)
	{
		std::cout << x;
	}

	return 0;
}