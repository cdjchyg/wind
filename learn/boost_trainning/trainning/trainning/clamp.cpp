
#include <array>
#include <boost/algorithm/clamp.hpp>

int test_main()
{
	auto foo = boost::algorithm::clamp(99, 1, 20, [](int a, int b) {return a > b; });
	auto foo1 = boost::algorithm::clamp(99, 1, 20, [](int a, int b) {return a < b; });

	std::array<int, 6> c1 = { 0, 1, 2, 3, 14, 15 };
	std::array<int, 6> c2;
	auto r = boost::algorithm::clamp_range(c1.begin(), c1.end(), c2.begin(), 1, 3);

	return 0;
}