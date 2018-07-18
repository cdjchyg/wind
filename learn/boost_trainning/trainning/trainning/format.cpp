#include <boost/format.hpp>  
#include <iostream>  

void test_main()
{
	std::cout << boost::format("writing %1%, x=%2% : %3%-th try") % "to" % 40.23 % 50 << std::endl;
	boost::format f("a=%1%, b=%2%, c=%3%, a=%1%");
	f % "string" % 2 % 10.0;
	std::cout << f << std::endl << f.str() << std::endl;
}