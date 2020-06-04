#include "stdafx.h"
#include "TCPClient.h"

TCPClient::TCPClient()
{
	//Initialize
	assert(Initialize());
	//Connect
	assert(Connect());
}

TCPClient::~TCPClient()
{
	//�ޱ� ���� ������ ����
	recv_thread->CloseThread();
	//���� ������ �� �� �����尡 �ִٸ� �����Ŵ
	for (int i = 0; i < 3; ++i)
	{
	    //���� ���� ������ ����
		send_thread[i]->CloseThread();
	}
	Close();
}

const bool TCPClient::Initialize()
{
	//���� �ʱ�ȭ
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return false;

	//���� �ʱ�ȭ
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		ErrorCheck::Get().err_quit("socket()");
		return false;
	}

	//socket_addr �ʱ�ȭ
	ZeroMemory(&socket_addr, sizeof(socket_addr));
	socket_addr.sin_family = AF_INET;
	socket_addr.sin_addr.s_addr = inet_addr(server_ip);
	socket_addr.sin_port = htons(server_port);

	return true;
}

bool TCPClient::Connect()
{
	char buf[BUFSIZE];

	//buf�� ������ �� ����
	ZeroMemory(buf, sizeof(buf));

	retval = connect(sock, (SOCKADDR *)&socket_addr, sizeof(socket_addr));
	if (retval == SOCKET_ERROR)
	{
		ErrorCheck::Get().err_quit("bind()");
		return false;
	}

	//Client ID�� ����
	retval = recv(sock, buf, BUFSIZE, 0);
	if (retval == SOCKET_ERROR)
	{
		ErrorCheck::Get().err_display("recv()");
		return false;
	}

	//Client ID�� ����
	client_id = Utility::GetClientIDFromData(buf);

	//Client ID���
	std::cout << "[" << client_id << "(" + Utility::GetMessageFromData(buf) + ")" + "]" << std::endl;

	//Shared Ptr ClientInfo����
	client_info = std::make_shared<ClientInfo*>(new ClientInfo(sock, socket_addr, client_id));

	//Ŭ���̾�Ʈ�� ������ Connect ������ �ϸ� �̸� ���ۿ� ����� ��������� �����Ҵ���
	send_thread[0] = new SendThread(*client_info, PT_CLIENT_CONNECT);
	send_thread[1] = new SendThread(*client_info, PT_CLIENT_SEND);
	send_thread[2] = new SendThread(*client_info, PT_CLIENT_DISCONNECT);
	recv_thread = new RecvThread(*client_info);

	//Connet ����
	//Send ClientConnectPacket
	send_thread[0]->StartThread();
	send_thread[0]->JoinThread();
	send_thread[0]->CloseThread();

	return true;
}

void TCPClient::CommunicateData()
{
	while (1)
	{
	    //���� ������2 ����
		send_thread[1]->StartThread();
		//�ޱ� ������ ����
		recv_thread->StartThread();
		//Ŭ���̾�Ʈ�� "/bye��" �����ͷ� �ѱ� ���
		//while���� ��������
		if (send_thread[1]->IsExitClient())
		{
			break;
		}
	}
	//���� ������2 ����
	send_thread[1]->CloseThread();
	//�ޱ� ������ ����
	recv_thread->CloseThread();
	//���� ������3 ����
	send_thread[2]->StartThread();
	//���� ������3�� ����� ������ ��ٸ�
	send_thread[2]->JoinThread();
	//���� ������3 ������ ����
	send_thread[2]->CloseThread();
	//���� ��������
	Close();
}

void TCPClient::Run()
{
	CommunicateData();
}

void TCPClient::Close()
{
	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();
}
