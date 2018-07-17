
#include<iostream>

#include "content.h"
#include "tun.h"
#include "util.h"

void TunWrite(SContent* pCont, char* buff, int len)
{
	char tunData[BUFF_SIZE];
	int sz = Decrypt(buff, len, tunData, BUFF_SIZE);
	if (sz > 0)
	{
		cout << "tun write data " << len << endl;
		pCont->mHand.write_some(boost::asio::buffer(tunData, sz));
	}
}

void NetSend(SContent* pCont, char* buff, int len)
{

	char sendData[BUFF_SIZE];
	int sz = Encrypt(buff, len, sendData, BUFF_SIZE);
	if (sz > 0)
	{
		cout << "net send data " << len << endl;
		pCont->mSocket.send_to(boost::asio::buffer(sendData, sz), pCont->mSvrEndPoint);
	}
}

void NetRead(SContent* pCont);
void NetReadHandle(const boost::system::error_code& errorCode,
	size_t readLen, SContent* pCont)
{
	if (errorCode)
	{
		cout << "net read to failed:" << errorCode << endl;
		return;
	}

	TunWrite(pCont, pCont->mNetReadBuff, (int)readLen);

	NetRead(pCont);
}

void NetRead(SContent* pCont)
{
	pCont->mSocket.async_receive_from(boost::asio::buffer(pCont->mNetReadBuff, BUFF_SIZE), pCont->mSvrEndPoint,
		boost::bind(NetReadHandle, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, pCont)
	);
}

void TunRead(SContent *pCont);
void HandleTunRead(const boost::system::error_code& errorCode,
	size_t readLen, SContent* pCont)
{
	if (errorCode)
	{
		cout << "tun read failed:" << errorCode << endl;
		return;
	}

	NetSend(pCont, pCont->mTunReadBuff, (int)readLen);

	TunRead(pCont);
}

void TunRead(SContent *pCont)
{
	pCont->mHand.async_read_some(boost::asio::buffer(pCont->mTunReadBuff, MAX_RECV_SIZE),
		boost::bind(HandleTunRead, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, pCont));
}


int main(int argc, char* argv[])
{
	string serverip = "";
	SContent content(serverip, 2200);
	ULONG index = -1;
	HANDLE hand = open_tun(index);
	if (!hand_defined(hand))
	{
		cout << "open tun error" << endl;
		return -1;
	}
	content.mHand.assign(hand);
	content.mAdaptIndex = index;
	adapter_config(hand, index, serverip);


	TunRead(&content);
	NetRead(&content);
	
	int i = 0;
	while (1)
	{
		content.ioContext.run_one();
	}
	return 0;
}