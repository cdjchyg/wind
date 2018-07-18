
#include <array>
#include <boost/algorithm/gather.hpp>

int test_main()
{
	std::array<int, 10> c = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

	auto ret = boost::algorithm::gather(c.begin(), c.end(), c.begin() + 4, [](int i) {return i > 7; });

	int ci[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	auto r = boost::algorithm::gather(ci, ci + 10, ci, [](int i) {return i > 7; });

	return 0;
}