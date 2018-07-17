#pragma once

#include <Ws2tcpip.h>
#include <windows.h>

bool hand_defined(HANDLE handle);
HANDLE open_tun(ULONG& index);
bool adapter_config(HANDLE handle, ULONG& adapterIndex, string serverip);