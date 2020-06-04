#pragma once

#define SERVERPORT 9000
#define BUFSIZE    512

class Session;

class TCPSuper
{
public:
	TCPSuper() = default;
	virtual ~TCPSuper() = default;

	//���� or Ŭ�� �ʱ�ȭ(WSADATA, SOCKET)
	virtual const bool Initialize() = 0;

	//���� or Ŭ�� ����
	virtual void Run() = 0;

	//���� or Ŭ�� ����
	virtual void Close() = 0;

	//Set&Get
	//WSADATA
	virtual auto GetWsa() const -> WSADATA = 0;

	//Socket
	virtual auto GetSocket() const -> SOCKET = 0;

	//Socket Address
	virtual auto GetSocketAddr() const -> SOCKADDR_IN = 0;

	//ServerPort
	virtual auto GetServerPort() const -> int = 0;
	virtual void SetServerPort(const int server_port) = 0;

protected:
	//���� �ʱ�ȭ ���� ����ü
	WSADATA wsa;
	//����
	SOCKET sock;
	//���� ������ �ּ�
	SOCKADDR_IN socket_addr;
	//Client ID
	std::string client_id = "Client";

	//������Ʈ
	int server_port = SERVERPORT;
	//�����͸� Recv�ϰų� Send�� �� ����üũ�� ���� ����
	int retval = 0;
	//�����Ͱ� ���������� send�Ǿ����� Ȯ���� ����
	bool result = false;
};