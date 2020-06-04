#pragma once

#define BUFSIZE   512
#include "../Thread/Thread.h"

//SessionClass
class Session : public Thread
{
public:
	virtual bool Run() = 0;
	void SendPackect(SOCKET& sock, PacketHeader* packet);
	void RecvPackect(SOCKET& sock, char* buf);
};

//Client Send
class SendThread : public Session
{
public:
	SendThread(ClientInfo* info, PacketType type) : client_info(info), packet_type(type) {};
	virtual bool Run();

	bool IsExitClient() { return is_exit; };

private:
	ClientInfo* client_info;
	PacketType packet_type;
	bool is_exit = false;
};

//Client Recv
class RecvThread : public Session
{
public:
	RecvThread(ClientInfo* info) : client_info(info) {};
	virtual bool Run();

private:
	ClientInfo* client_info;
};