#pragma once
#include<string>
#include<vector>

#ifdef HOLDMINE_EXPORTS
#define DECLSPEC __declspec(dllexport)
#else
#define DECLSPEC __declspec(dllimport)
#endif


// ��Ŷ�� ���� �� �˷��ִ� Ŭ����
class DECLSPEC NotificationOnPacketHandler
{
public:
	NotificationOnPacketHandler(void) { }
	virtual ~NotificationOnPacketHandler(void) {}

public:
	virtual void onPacket(const char *buf, int len, int flags) {}
	virtual void onParsingPacket(void) {}
};

class DECLSPEC HookPackets
{
public:
	static HookPackets* instance(void) { return &HookPackets::GlobalVariant; }
	static HookPackets GlobalVariant;
public:
	HookPackets() {
		toServer = nullptr;
		toClient = nullptr;
	}
	virtual ~HookPackets(){};

	void setNotification(NotificationOnPacketHandler *send, NotificationOnPacketHandler *recv)
	{
		toServer = send;
		toClient = recv;
	}

public:
	NotificationOnPacketHandler *toServer;
	NotificationOnPacketHandler *toClient;

};