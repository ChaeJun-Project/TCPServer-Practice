#include "stdafx.h"
#include "Session.h"

//Session
//===============================================================
void Session::SendPackect(SOCKET & sock, PacketHeader * packet)
{
	char buffer[BUFSIZE];
	int result;

	//string ->const char* �� ��ȯ �� ������� buf�� ����
	strcpy(buffer, Convert::ConvertStringToChar(packet->GetDataMessage()));

	result = send(sock, buffer, packet->GetPacketSize(), 0);
	if (result == SOCKET_ERROR)
	{
		ErrorCheck::Get().err_display("send()");
		return;
	}
}

void Session::RecvPackect(SOCKET & sock, char * buf)
{
	int result;

	result = recv(sock, buf, BUFSIZE, 0);
	if (result == SOCKET_ERROR)
	{
		ErrorCheck::Get().err_display("recv()");
		return;
	}
}
//===============================================================

//SendThread
//===============================================================
bool SendThread::Run()
{
	bool result = true;

	switch (packet_type)
	{
	case PT_CLIENT_CONNECT:
	{
		PacketHeader* packet = new PacketConnect(client_info->client_id);
		SendPackect(client_info->client_sock, packet);
	}
	break;
	case PT_CLIENT_DISCONNECT:
	{
		PacketHeader* packet = new PacketDisConnect(client_info->client_id);
		SendPackect(client_info->client_sock, packet);
	}
	break;
	case PT_CLIENT_SEND:
	{
		char buffer[BUFSIZE];
		std::cin >> buffer;

		if (!strcmp(buffer, "/bye"))
		{
			is_exit = result;
			return result;
		}

		PacketHeader* packet = new PacketClientSend(client_info->client_id, Convert::ConvertCharToString(buffer));
		SendPackect(client_info->client_sock, packet);
	}
	break;
	}

	return result;
}
//===============================================================

//RecvThread
//===============================================================
bool RecvThread::Run()
{
	bool result = true;
	char buffer[BUFSIZE];

	RecvPackect(client_info->client_sock, buffer);

	//���� �޽����� ���� ��
	if (Convert::ConvertCharToString(buffer).size() == 0)
	{
		result = false;
		return result;
	}

	//���� �޽����� ���� ��
	else
	{
		std::cout << buffer << std::endl;
		return true;
	}
}
//===============================================================
