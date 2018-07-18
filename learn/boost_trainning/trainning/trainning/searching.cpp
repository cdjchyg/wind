
#include <string>

#include <boost/algorithm/searching/boyer_moore.hpp>

int test_main()
{
	std::string corpusText("this is test string");
	std::string patternText("test");

	auto findResult = boost::algorithm::boyer_moore_search(corpusText.begin(), corpusText.end(),
		patternText.begin(), patternText.end());

	std::string resultStr = std::string(findResult.first, findResult.second);

	std::cout << "function way:" << resultStr << std::endl;

	boost::algorithm::boyer_moore<std::string::iterator> patternObj(patternText.begin(), patternText.end());
	auto objFindResult = patternObj(corpusText.begin(), corpusText.end());

	std::cout << "object way:" << std::string( objFindResult.first, objFindResult.second ) << std::endl;

	/*
		boyer_moore_horspool 

		class boyer_moore_horspool;
		function boyer_moore_horspool_search;
	*/

	/*
		knuth_morris_pratt

		class knuth_morris_pratt;
		function knuth_morris_pratt_search;
	*/

	return 0;
}