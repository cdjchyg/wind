
#include <array>
#include "boost/algorithm/is_partitioned_until.hpp"

int test_main()
{
	std::array<int, 6> c = { 0, 1, 2, 3, 14, 15 };

	auto isOdd = [](int i) {return i % 2 == 1; };
	auto lessThan10 = [] (int i) {return i < 10; };

	auto r = boost::algorithm::is_partitioned_until(c, isOdd);
	auto r2 = boost::algorithm::is_partitioned_until(c, lessThan10);

	return 0;
}