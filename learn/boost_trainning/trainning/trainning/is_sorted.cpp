
#include <array>
#include <boost/algorithm/cxx11/is_sorted.hpp>

int test_main()
{
	std::array<int, 6> c = { 0, 1, 2, 3, 14, 15 };
	auto isSort = boost::algorithm::is_sorted(c);

	std::array<int, 7> c2 = { 0, 1, 2, 3, 30, 14, 15 };
	auto res = boost::algorithm::is_sorted_until(c2);

	//is_increasing Pred:less
	//is_decreasing Pred:greater
	//is_strictly_increasing Pred:less_equal
	//is_strictly_decreasing Pred:greater_equal

	auto isInc = boost::algorithm::is_increasing(c);
	auto isDec = boost::algorithm::is_decreasing(c);
	auto isStrictlyInc = boost::algorithm::is_strictly_increasing(c);
	auto isStrictlyDec = boost::algorithm::is_strictly_decreasing(c);

	return 0;
}