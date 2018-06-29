
#include<iostream>
#include<boost/bind.hpp>
#include<boost/asio.hpp>

#include <windows.h>
#include <iphlpapi.h>

#pragma comment(lib, "iphlpapi.lib")

using namespace boost::asio;
using namespace boost::asio::ip;
using namespace std;

#define EDECRY_MASK 128

#define ADAPTER_KEY "SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002BE10318}"
#define COMPONENT_ID "tap0901"
#define USERMODEDEVICEDIR "\\\\.\\Global\\"
#define TAP_WIN_SUFFIX    ".tap"


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
					return hand;
				}
			}
		}
	}

	return INVALID_HANDLE_VALUE;
}

class CClientNet
{
public:
	CClientNet(io_context& io_content, const string& svrip, const short port)
		: mSvrEndPoint(ip::address::from_string(svrip), port)
		, mSocket(io_content, udp::endpoint(udp::v4(), 2200))
	{
		char buff[] = "gtopen";
		//Send(buff, strlen(buff));
	}

	void Send(char* buff, int len)
	{
		for (int i = 0; i < len; ++i)
		{
			buff[i] ^= EDECRY_MASK;
		}

		mSocket.async_send_to(
			boost::asio::buffer(buff, len)
			, mSvrEndPoint
			, boost::bind(&CClientNet::HandleSendTo, this,
				boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
		);
	}

	void HandleSendTo(const boost::system::error_code& errorCode,
		size_t sendLen)
	{
		if (errorCode)
		{
			cout << "Send to failed:" << errorCode << endl;
		}
		else
		{
			//cout << "Send to suc:" << sendLen << endl;
		}
	}

private:
	udp::socket mSocket;
	udp::endpoint mSvrEndPoint;
	char mData[1024];
};



int main(int argc, char* argv[])
{
	HANDLE hand = open_tun();

	io_context ioContext;

	CClientNet net(ioContext, "104.225.145.30", 2205);
	char buff[127];
	int i = 0;
	while (1)
	{
		ioContext.run_one();

		sprintf(buff, "hello world:%d", i++);
		cout << "send content:" << buff << endl;
		net.Send(buff, strlen(buff));
		Sleep(1000);
	}
	return 0;
}