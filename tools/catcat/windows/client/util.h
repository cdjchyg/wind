#pragma once
#include <string>

using namespace std;

int Decrypt(char* in, int inLen, char* out, int outLen);
int Encrypt(char* in, int inLen, char* out, int outLen);
char* ToString(unsigned long);
int CmdRun(string command);