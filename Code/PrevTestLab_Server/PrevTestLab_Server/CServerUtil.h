#pragma once
#include <iostream>
#include <winsock2.h>

void err_quit(char *msg);
void err_quit(const char *msg);
void err_display(char *msg);
void err_display(const char *msg);
BOOL GetIPAddr(char *name, IN_ADDR *addr);
BOOL GetDomainName(IN_ADDR addr, char *name, int namelen);
int recvn(SOCKET s, char *buf, int len, int flags);

class CServerUtil
{
public:
	CServerUtil();
	~CServerUtil();
};

