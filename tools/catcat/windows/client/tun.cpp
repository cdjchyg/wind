
#include "tun.h"

#include<iostream>

#include <winioctl.h>
#include <iphlpapi.h>

#include "tap-windows.h"
#include "util.h"

#pragma comment(lib, "iphlpapi.lib")

#define COMPONENT_ID "tap0901"

using namespace std;

bool hand_defined(HANDLE handle)
{
	return handle != nullptr && handle != INVALID_HANDLE_VALUE;
}

HANDLE open_tun(ULONG& index)
{
	HKEY adapter_key;
	LONG status = ::RegOpenKeyExA(HKEY_LOCAL_MACHINE, ADAPTER_KEY, 0, KEY_READ, &adapter_key);
	if (status != ERROR_SUCCESS)
	{
		cout << "reg open key failed:" << status << endl;
		exit(-1);
	}

	for (int i = 0;; ++i)
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
			index = aindex;
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

bool adapter_config(HANDLE handle, ULONG& adapterIndex, string serverip)
{

	//参考 openvpn3 adapter_config

	string index = ToString(adapterIndex);

	{
		DWORD len;
		ULONG status = true;
		if (!DeviceIoControl(handle, TAP_WIN_IOCTL_SET_MEDIA_STATUS,
			&status, sizeof(status),
			&status, sizeof(status), &len, NULL))
			return false;
	}

	if (CmdRun("netsh interface ip set interface " + index + " metric=1") < 0)
		return false;

	{
		//#1
		in_addr ls, rs;
		int ret = inet_pton(AF_INET, "192.168.0.1", (void*)&ls);
		if (ret < 0)
			return false;
		ret = inet_pton(AF_INET, "192.168.0.2", (void*)&rs);
		if (ret < 0)
			return false;
		uint32_t ep[2];
		ep[0] = ls.s_addr;//local
		ep[1] = rs.s_addr;//remote
		DWORD len;
		if (!::DeviceIoControl(handle, /*TAP_WIN_IOCTL_CONFIG_TUN, */ TAP_WIN_IOCTL_CONFIG_POINT_TO_POINT,
			ep, sizeof(ep),
			ep, sizeof(ep), &len, nullptr
		))
			return false;
	}

	if (CmdRun("netsh interface ip set address " + index + " static 192.168.0.1 255.255.255.255 store=active") < 0)
		return false;

	// 在#1中会添加改路由
	//if (CmdRun("netsh interface ip add route 192.168.0.2/32 " + index + " 192.168.0.1 store=active") < 0)
	//	return false;

	//if (CmdRun("netsh interface ip add route 0.0.0.0/0 " + index + " 192.168.0.1 metric=1 store=active") < 0)
	//	return false;

	//if (CmdRun("netsh interface ip add route " + serverip + "/32 11 192.168.55.1 store=active") < 0)
	//	return false;

	if (CmdRun("netsh interface ip add dnsservers " + index + " 8.8.8.8") < 0)
		return false;

	//if (CmdRun("ipconfig /flushdns") < 0)
	//	return false;

	Sleep(1000);

	return true;
}
