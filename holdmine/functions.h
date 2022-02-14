#pragma once
#include <Windows.h>
#include <winsock.h>
#include <string>
#include <fstream>

#ifdef HOLDMINE_EXPORTS
#define DECLSPEC __declspec(dllexport)
#else
#define DECLSPEC __declspec(dllimport)
#endif


extern "C" DECLSPEC int __stdcall Real_Recv(
	_In_  SOCKET s,
	_Out_ char   *buf,
	_In_  int    len,
	_In_  int    flags
	);
extern "C" DECLSPEC int __stdcall Real_Send(
	_In_       SOCKET s,
	_In_ const char   *buf,
	_In_       int    len,
	_In_       int    flags
	);


extern "C" DECLSPEC void* Get_Recv();
extern "C" DECLSPEC void* Get_Send();

void PutJMP(void* WriteTo, void* HookFunction);

extern "C" DECLSPEC int __stdcall Hook_Recv(
	_In_  SOCKET s,
	_Out_ char   *buf,
	_In_  int    len,
	_In_  int    flags
	);
extern "C" DECLSPEC int __stdcall Hook_Send(
	_In_       SOCKET s,
	_In_ const char   *buf,
	_In_       int    len,
	_In_       int    flags
	);





/*
char buf[INET_ADDRSTRLEN];
struct sockaddr name; socklen_t len = sizeof(name);
getpeername(sock_fd, &name, &len); //sock_fd is the socket im using
inet_ntop(AF_INET, &name, buf, INET_ADDRSTRLEN);
string ip = buf;
*/