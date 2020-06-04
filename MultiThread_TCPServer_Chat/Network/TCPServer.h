#pragma once

#include "TCPSuper.h"

class TCPServer final : public TCPSuper
{
public:
	TCPServer();
	~TCPServer();

	//���� �ʱ�ȭ(WSADATA, SOCKET)
	const bool Initialize() override;

	//Server
	//==================================================================
	//Bind
	bool Bind();

	//listen
	bool Listen();

	//Accept
	void Accept();

	//Create Client ID
	void CreateClientID();

	//���� -> Ŭ��� ���� ������ �ѱ�
	void SendAccept(const std::string& client_id);

	//���� -> Ŭ��� Ư�� Ŭ�� ���� ���� ������ �ѱ�
	void SendDisAccept(const std::string& client_id);

	//Write ServerLog.txt
	void WriteFile(const std::string& log);

	//���� -> Ŭ��� �����͸� �ѱ�
	void SendData(const char * buf);

	//�������� Ŭ���̾�Ʈ�� ������ ���, Accept�Լ����� ������� ����ó��
	void CommunicateData(const std::string& client_id);

	//������ ������ Ŭ���̾�Ʈ�� �ּҿ� ��Ʈ��ȣ�� ���
	void PrintIPAndPort(const SOCKADDR_IN client_addr);
	//==================================================================

	//���� ����
	void Run() override;

	//���� ���� ����
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
	//Client�� ������ ����
	void AddClientInfo(ClientInfo* client_info);

	//����� Client�� ������ Client_id�� ���� ����
	void DeleteClientInfo(const std::string& client_id);

	//����� Client�� ������ Client_id�� ���� ��ȯ
	ClientInfo* GetClientInfo(const std::string& client_id) const;

	//������ ��ſ� ����� ����
	//ó���� Accept�� ������ Ŭ���̾�Ʈ�� ���ϰ��� ������ ����
	SOCKET client_sock;
	//Accept�� ������ Ŭ���̾�Ʈ�� ���� �ּҰ��� ������ ����
	SOCKADDR_IN client_addr;
	//Ŭ���̾�Ʈ�� ����
	int client_num = 0;
	int addrlen;

	//����� Ŭ���̾�Ʈ�� ������ ������ vector
	std::vector<ClientInfo*> client_infos;

	//�����α� �ؽ�Ʈ ����
	std::string server_log = "ServerLog.txt";

	//���� �����͸� ������ ����
	char buf[BUFSIZE];

	//����ȭ�� ���� mutex
	std::mutex mutex;
};