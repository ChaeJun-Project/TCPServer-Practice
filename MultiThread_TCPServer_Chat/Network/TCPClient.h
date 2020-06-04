#pragma once

#include "TCPSuper.h"
#include "../Session/Session.h"

class TCPClient final : public TCPSuper
{
public:
	//�����ڿ��� Initialize �� Connet �Լ� ȣ��
	TCPClient();
	~TCPClient();

	//Ŭ�� �ʱ�ȭ(WSADATA, SOCKET)
	const bool Initialize() override;

	//Client
	//==================================================================
	//Connect
	bool Connect();

	//Ŭ�󿡼� ������ ������ ���
	void CommunicateData();
	//==================================================================

	//Ŭ�� ����
	void Run() override;

	//Ŭ�� ���� ����
	void Close() override;

	//Set&Get
	//WSADATA
	auto GetWsa() const -> WSADATA  override { return wsa; }

	//Socket
	auto GetSocket() const -> SOCKET  override { return sock; }

	//Socket Address
	auto GetSocketAddr() const -> SOCKADDR_IN  override { return socket_addr; }

	//ServerPort
	auto GetServerPort() const -> int  override { return server_port; }
	void SetServerPort(const int server_port)  override { this->server_port = server_port; }


private:
	const char* server_ip = "127.0.0.1";

	//�� Ŭ���̾�Ʈ�� �ڽ��� ������ ���� client_info�� ����Ʈ�����ͷ� ����.
	//Send �� Recv������� ������ �� client_info���� �����ϱ� ������ �����ϰ� �����ϱ� ���ؼ�
	//shared ptr���
	std::shared_ptr<ClientInfo*> client_info = nullptr;

	//������ ������ ���� ������
	//3���� ������ Connect, DisConnect, Send �� 3���� ��Ŷ Ÿ���� �ֱ� ������
	SendThread* send_thread[3];
	//�����͸� �ޱ� ���� ������
	RecvThread* recv_thread;
};
