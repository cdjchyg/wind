
#include<iostream>
#include<list>

#include<boost/bind.hpp>
#include<boost/asio.hpp>
#include<boost/asio/windows/stream_handle.hpp>

#include <windows.h>
#include <winioctl.h>
#include <iphlpapi.h>


#include "tap-windows.h"

#pragma comment(lib, "iphlpapi.lib")

using namespace boost::asio;
using namespace boost::asio::ip;
using namespace std;

#define EDECRY_MASK 128
#define BUFF_SIZE 1096

#define COMPONENT_ID "ptun0901"

struct SData
{
	SData()
		:mSize(0)
		, mData(NULL)
	{

	}
	SData(char* buff, int size)
	{
		if (!buff || size < 1)
		{
			return;
		}
		mSize = size;
		mData = new char[size];
		strncpy(mData, buff, size);
	}
	~SData()
	{
		if (mData)
		{
			delete mData;
			mData = NULL;
		}
	}
	int mSize;
	char* mData;

};

struct SContent
{
	SContent(const string& svrip, const short port)
		: mHand(ioContext)
		, mSocket(ioContext, udp::endpoint(udp::v4(), 2200))
		, mSvrEndPoint(ip::address::from_string(svrip), port)
		, mNetIsSending(false)
		, mTunIsWriteing(false)
		, mNetCurSending(NULL)
		, mTunCurWriting(NULL)
	{

	}

	io_context ioContext;
	windows::stream_handle mHand;
	udp::socket mSocket;
	udp::endpoint mSvrEndPoint;

	char mTunReadBuff[BUFF_SIZE];
	char mNetReadBuff[BUFF_SIZE];

	list<SData*> mTun2NetList;
	list<SData*> mNet2TunList;

	SData* mNetCurSending;
	SData* mTunCurWriting;

	bool mNetIsSending;
	bool mTunIsWriteing;
};


bool hand_defined(HANDLE handle)
{
	return handle != nullptr && handle != INVALID_HANDLE_VALUE;
}

HANDLE open_tun()
{
	HKEY adapter_key;
	LONG status = ::RegOpenKeyExA(HKEY_LOCAL_MACHINE, ADAPTER_KEY, 0, KEY_READ, &adapter_key);
	if (status != ERROR_SUCCESS)
	{
		cout << "reg open key failed:" << status << endl;
		exit(-1);
	}

	for (int i = 0;;++i)
	{
		char buff[256];
		DWORD len = sizeof(buff);
		status = ::RegEnumKeyExA(adapter_key, i, buff, &len, NULL, NULL, NULL, NULL);
		if (status == ERROR_NO_MORE_ITEMS)
		{
			break;
		}
		buff[len] = '\0';

		HKEY unitKey;
		string unitStr = string(ADAPTER_KEY) + "\\" + buff;
		status = ::RegOpenKeyExA(HKEY_LOCAL_MACHINE, unitStr.c_str(), 0, KEY_READ, &unitKey);
		if (status != ERROR_SUCCESS)
		{
			continue;
		}

		DWORD data_type;
		len = sizeof(buff);
		status = ::RegQueryValueExA(unitKey, "ComponentId", NULL, &data_type, (LPBYTE)buff, &len);
		if (status != ERROR_SUCCESS || data_type != REG_SZ)
		{
			continue;
		}
		buff[len] = '\0';

		if (std::strcmp(buff, COMPONENT_ID))
		{
			continue;
		}

		len = sizeof(buff);
		status = ::RegQueryValueExA(unitKey, "NetCfgInstanceId", NULL, &data_type, (LPBYTE)buff, &len);
		if (status == ERROR_SUCCESS && data_type == REG_SZ)
		{
			buff[len] = '\0';
		}

		{
			string guid(buff);
			ULONG aindex;
			const size_t len = 128;
			wchar_t wbuf[len];
			_snwprintf(wbuf, len, L"\\DEVICE\\TCPIP_%S", guid.c_str());
			DWORD ret = ::GetAdapterIndex(wbuf, &aindex);
			if (ret == NO_ERROR)
			{
				string tapPath = std::string(USERMODEDEVICEDIR) + guid + TAP_WIN_SUFFIX;
				HANDLE hand = ::CreateFileA(tapPath.c_str(), GENERIC_READ | GENERIC_WRITE,
					0,
					0,
					OPEN_EXISTING,
					FILE_ATTRIBUTE_SYSTEM | FILE_FLAG_OVERLAPPED,
					0
				);

				if (hand_defined(hand))
				{
					ULONG info[3];
					DWORD len;
					info[0] = info[1] = info[2] = 0;
					bool res = ::DeviceIoControl(hand, TAP_WIN_IOCTL_GET_VERSION,
						&info, sizeof(info),
						&info, sizeof(info), &len, NULL);
					if (!res)
					{
						continue;
					}

					cout << "tap win v:" << info[0] << '.' << info[1];
					if (info[2])
						cout << " (DEBUG)";

					ULONG mtu;
					res = DeviceIoControl(hand, TAP_WIN_IOCTL_GET_MTU,
						&mtu, sizeof(mtu),
						&mtu, sizeof(mtu), &len, NULL);
					if (!res)
						continue;
					cout << "TAP-Windows MTU=" << (int)mtu;


					return hand;
				}
			}
		}
	}

	return INVALID_HANDLE_VALUE;
}

void TunWriteNext(SContent* pCont);

void HandleTunWrite(const boost::system::error_code& errorCode,
	size_t writeLen, SContent* pCont)
{
	if (errorCode)
	{
		cout << "tun write failed:" << errorCode << endl;
		return;
	}

	delete pCont->mTunCurWriting;
	pCont->mTunCurWriting = NULL;

	TunWriteNext(pCont);
}

void TunWriteNext(SContent* pCont)
{
	list<SData*>& writeList = pCont->mNet2TunList;
	if (writeList.size() < 1)
	{
		pCont->mTunIsWriteing = false;
		return;
	}

	pCont->mTunIsWriteing = true;
	SData* pData = writeList.front();
	writeList.pop_front();

	pCont->mTunCurWriting = pData;

	for (int i = 0; i < pData->mSize; ++i)
	{
		pData->mData[i] ^= EDECRY_MASK;
	}

	pCont->mHand.async_write_some(boost::asio::buffer(pData->mData, pData->mSize),
		boost::bind(HandleTunWrite, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, pCont)
	);
}

void TunWrite(SContent* pCont, char* buff, int len)
{
	pCont->mNet2TunList.push_back(new SData(pCont->mNetReadBuff, len));
	if (!pCont->mTunIsWriteing)
	{
		TunWriteNext(pCont);
	}
}

void NetSendNext(SContent* pCont);
void NetSendHandle(const boost::system::error_code& errorCode,
	size_t sendLen, SContent* pCon)
{
	if (errorCode)
	{
		cout << "net Send to failed:" << errorCode << endl;
		return;
	}

	delete pCon->mNetCurSending;
	pCon->mNetCurSending = NULL;

	NetSendNext(pCon);
}

void NetSendNext(SContent* pCont)
{
	list<SData*>& sendList = pCont->mTun2NetList;
	if (sendList.size() < 1)
	{
		pCont->mNetIsSending = false;
		return;
	}
	pCont->mNetIsSending = true;
	SData* pData = sendList.front();
	sendList.pop_front();

	pCont->mNetCurSending = pData;

	for (int i = 0; i < pData->mSize; ++i)
	{
		pData->mData[i] ^= EDECRY_MASK;
	}

	pCont->mSocket.async_send_to(
		boost::asio::buffer(pData->mData, pData->mSize)
		, pCont->mSvrEndPoint
		, boost::bind(NetSendHandle,
			boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, pCont)
	);
}

void NetSend(SContent* pCont, char* buff, int len)
{
	pCont->mTun2NetList.push_back(new SData(buff, len));

	if (!pCont->mNetIsSending)
	{
		NetSendNext(pCont);
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

	TunWrite(pCont, pCont->mNetReadBuff, readLen);

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

	NetSend(pCont, pCont->mTunReadBuff, readLen);

	TunRead(pCont);
}

void TunRead(SContent *pCont)
{
	pCont->mHand.async_read_some(boost::asio::buffer(pCont->mTunReadBuff, BUFF_SIZE),
		boost::bind(HandleTunRead, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, pCont));
}


int main(int argc, char* argv[])
{
	SContent content("104.225.145.30", 2205);
	HANDLE hand = open_tun();
	if (!hand_defined(hand))
	{
		cout << "open tun error" << endl;
		return -1;
	}
	content.mHand.assign(hand);
	TunRead(&content);

	char buff[] = "gtopen";
	NetSend(&content, buff, sizeof(buff));
	NetRead(&content);
	
	int i = 0;
	while (1)
	{
		content.ioContext.run_one();
	}
	return 0;
}