
#include "util.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>

char xor_magic_1 = (char)220;
char xor_magic_2 = (char)171;

char XORHeader(char* buff, int len)
{
	char c = 0;
	for (size_t i = 0; i < len; i++)
	{
		c ^= buff[i];
	}
	return c;
}

int XORData(char* inOutBuff, int len, char magic1, char magic2)
{
	for (int i = 0; i < len; ++i)
	{
		inOutBuff[i] ^= magic1;
		inOutBuff[i] ^= magic2;
	}

	return len;
}

int Decrypt(char* in, int inLen, char* out, int outLen)
{
	int sz = XORData(in, inLen, xor_magic_2, xor_magic_1);
	--sz;
	if (sz < 0 || outLen < sz)
	{
		return -1;
	}

	if (in[0] != XORHeader(in + 1, sz))
	{
		return -1;
	}

	memcpy(out, in + 1, sz);

	return sz;
}

int Encrypt(char* in, int inLen, char* out, int outLen)
{
	if (inLen + 1 > outLen)
	{
		return -1;
	}
	out[0] = XORHeader(in, inLen);
	memcpy(out + 1, in, inLen);

	return XORData(out, inLen + 1, xor_magic_1, xor_magic_2);
}

char* ToString(unsigned long n)
{
	static char buff[32];

	sprintf(buff, "%u", n);

	return buff;
}

int CmdRun(string command)
{
	return system(command.c_str());
}
