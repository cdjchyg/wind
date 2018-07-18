

#include <boost/asio/yield.hpp>
#include <iostream>

class TestCoro1 :boost::asio::coroutine
{
private:
	int mi;
public:
	TestCoro1()
		:mi(-1)
	{

	}

	bool slice()
	{
		std::cout << mi;

		reenter(this)
		{
			mi = 0;
			for (; mi < 1; ++mi)
			{
				std::cout << "yield" << mi;
				yield;


				//fork;
			}
		}

		return is_complete();
	}
};

class TestCoro :boost::asio::coroutine
{
private:
	int mi;
public:
	TestCoro()
		:mi(-1)
	{

	}

	bool slice()
	{
		std::cout << mi;

		//reenter(this)
		////////////////////
		switch(::boost::asio::detail::coroutine_ref _coro_value = this) 
    case -1: if (_coro_value) 
		{ 
			goto terminate_coroutine; 
			terminate_coroutine: 
			_coro_value = -1; 
			goto bail_out_of_coroutine; 
			bail_out_of_coroutine: 
			break; 
		} 
			 else case 0:
		////////////////////

		{
			
			mi = 0;
			
			for (; mi < 10; ++mi)
			{
				std::cout << "yield" << mi;


				//yield;
				////////////////////
				for (_coro_value = (__LINE__);;)
					if (_coro_value == 0) 
					{ 
			 case (__LINE__) : ;
						break; 
					} 
					else 
						switch (_coro_value ? 0 : 1) 
							for (;;) 
			 case -1: if (_coro_value) 
								goto terminate_coroutine; 
					  else for (;;) 
			 case 1: if (_coro_value) 
								goto bail_out_of_coroutine; 
					 else case 0:;

					 if (0)
					 {
						 ////////////////////
							  // fork;
							  //////////////
						 for (_coro_value = -(__LINE__);; _coro_value = (__LINE__))
							 if (_coro_value == (__LINE__))
							 {
					 case -(__LINE__) : ;
						 break;
							 }
							 else;
							 //////////////
					 }
			}
		}

		return is_complete();
	}
};

void test_main()
{
	
	TestCoro coro;
	//TestCoro1 coro;
	do
	{
		bool b = coro.slice();
		if (b)
		{
			break;
		}
	} while (1);
}