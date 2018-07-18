#include <array>
#include <iostream>
#include <boost/algorithm/cxx11/one_of.hpp>

//The function none_of returns true if the predicate returns false for every item in the sequence.

int test_main()
{
	std::array<int, 6> c = { 0, 1, 2, 3, 14, 15 };

	auto isOdd = [](int i)->bool {return i % 2 == 1; };
	auto lessThan10 = [](int i)->bool {return i < 10; };

	std::cout << boost::algorithm::one_of(c, isOdd) << std::endl;
	std::cout << boost::algorithm::one_of(c.begin(), c.end(), lessThan10) << std::endl;
	std::cout << boost::algorithm::one_of(c.end(), c.end(), lessThan10) << std::endl;

	std::cout << boost::algorithm::one_of_equal(c, 3) << std::endl;
	std::cout << boost::algorithm::one_of_equal(c.begin() + 3, c.begin() + 4, 3) << std::endl;
	std::cout << boost::algorithm::one_of_equal(c.begin(), c.begin(), 99) << std::endl;
	return 0;
}