#include <boost/thread/thread.hpp>
#include <boost/lockfree/queue.hpp>
#include <iostream>

#include <boost/atomic.hpp>

/*
* http://www.cnblogs.com/catch/tag/%E6%97%A0%E9%94%81%E9%98%9F%E5%88%97/
*/

boost::atomic_int producer_count(0);
boost::atomic_int consumer_count(0);

boost::lockfree::queue<int> queue(3);

const int iterations = 10000000;
const int producer_thread_count = 2;
const int consumer_thread_count = 0;

void producer(void)
{
	for (int i = 0; i != iterations; ++i) {
		int value = ++producer_count;
		while (!queue.push(value))
			;
	}
}

boost::atomic<bool> done(false);
void consumer(void)
{
	int value;
	while (!done) {
		while (queue.pop(value))
			++consumer_count;
	}

	while (queue.pop(value))
		++consumer_count;
}

int test_main(/*int argc, char* argv[]*/)
{
	using namespace std;
	cout << "boost::lockfree::queue is ";
	if (!queue.is_lock_free())
		cout << "not ";
	cout << "lockfree" << endl;

	boost::thread_group producer_threads, consumer_threads;

	for (int i = 0; i != producer_thread_count; ++i)
		producer_threads.create_thread(producer);

	for (int i = 0; i != consumer_thread_count; ++i)
		consumer_threads.create_thread(consumer);

	producer_threads.join_all();
	done = true;

	consumer_threads.join_all();

	cout << "produced " << producer_count << " objects." << endl;
	cout << "consumed " << consumer_count << " objects." << endl;

	return 0;
}