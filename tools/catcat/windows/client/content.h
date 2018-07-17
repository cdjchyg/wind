#pragma once

#include <string>
#include<boost/bind.hpp>
#include<boost/asio.hpp>
#include<boost/asio/windows/stream_handle.hpp>

using namespace boost::asio;
using namespace boost::asio::ip;

using namespace std;

#define BUFF_SIZE 2000
#define MAX_RECV_SIZE (BUFF_SIZE - 1)

struct SContent
{
	SContent(const string& svrip, const short port)
		: mHand(ioContext)
		, mSocket(ioContext, udp::endpoint(udp::v4(), 2200))
		, mSvrEndPoint(ip::address::from_string(svrip), port)
	{

	}

	io_context ioContext;
	windows::stream_handle mHand;
	ULONG mAdaptIndex;
	udp::socket mSocket;
	udp::endpoint mSvrEndPoint;

	char mTunReadBuff[BUFF_SIZE];
	char mNetReadBuff[BUFF_SIZE];
};
