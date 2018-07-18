
#include <boost/algorithm/hex.hpp>

int test_main()
{
	char out[255];
	auto r1 = boost::algorithm::hex("32", out); // height and low 4 byte to hex
	auto r2 = boost::algorithm::unhex("3332", out);

	return 0;
}