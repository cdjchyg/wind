//
// main.cpp
// ~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <signal.h>
#include "server.hpp"
#include "file_handler.hpp"

int test_main(int argc, char* argv[])
{
  try
  {
    // Check command line arguments.
    //if (argc != 4)
    //{
    //  std::cerr << "Usage: http_server <address> <port> <doc_root>\n";
    //  std::cerr << "  For IPv4, try:\n";
    //  std::cerr << "    receiver 0.0.0.0 80 .\n";
    //  std::cerr << "  For IPv6, try:\n";
    //  std::cerr << "    receiver 0::0 80 .\n";
    //  return 1;
    //}

    boost::asio::io_service io_service;

    // Launch the initial server coroutine.
    http::server4::server(io_service, "127.0.0.1", "2222",
        http::server4::file_handler("e:/"))();

    // Wait for signals indicating time to shut down.
    boost::asio::signal_set signals(io_service);
    signals.add(SIGINT);
    signals.add(SIGTERM);
#if defined(SIGQUIT)
    signals.add(SIGQUIT);
#endif // defined(SIGQUIT)
    signals.async_wait(boost::bind(
          &boost::asio::io_service::stop, &io_service));

    // Run the server.
    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "exception: " << e.what() << "\n";
  }

  return 0;
}
