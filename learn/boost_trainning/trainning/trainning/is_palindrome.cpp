

#include <boost/algorithm/is_palindrome.hpp>

int test_main()
{
	int palinderome[] = {1, 2, 3, 4, 3, 2, 1};

	auto r = boost::algorithm::is_palindrome(palinderome);

	return 0;
}