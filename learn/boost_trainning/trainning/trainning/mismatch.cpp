#include <array>
#include <boost/algorithm/cxx14/mismatch.hpp>

int test_main()
{
	std::array<int, 6> c1 = { 0, 1, 2, 3, 14, 15 };
	std::array<int, 3> c2 = { 1, 2, 3 };

	auto r = boost::algorithm::mismatch(c1.begin(), c1.end(), c2.begin(), c2.end());
	r = boost::algorithm::mismatch(c1.begin() + 1, c1.begin() + 4, c2.begin(), c2.end());
	r = boost::algorithm::mismatch(c1.end(), c1.end(), c2.end(), c2.end());

	return 0;
}